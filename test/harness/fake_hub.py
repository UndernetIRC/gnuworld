"""Listening fake ircu P10 hub for GNUWorld integration tests."""

from __future__ import annotations

import asyncio
import logging
import ssl
import time
from collections.abc import Callable
from pathlib import Path
from typing import Any

from p10 import (
    client_numnick,
    int_to_b64,
    ipv4_to_b64,
    p10_token,
    server_numeric,
    strip_msg_tags,
)

logger = logging.getLogger("fake_hub")

Predicate = Callable[[str], bool]


class FakeHub:
    """A fake IRC hub that listens and speaks P10 to an inbound GNUWorld.

    Lifecycle:
      await hub.start()
      # start gnuworld pointed at hub.host:hub.port
      await hub.accept_and_handshake()
      ...
      await hub.close()
    """

    def __init__(
        self,
        name: str = "hub.testnet",
        numeric: int = 1,
        password: str = "testpass",
        max_clients: int = 262143,
        description: str = "Fake Hub",
        server_flags: str = "hs",
        host: str = "0.0.0.0",
        *,
        tls: bool = False,
        tls_certfile: str | Path | None = None,
        tls_keyfile: str | Path | None = None,
    ):
        self.name = name
        self.numeric = numeric
        self.password = password
        self.max_clients = max_clients
        self.description = description
        self.server_flags = server_flags
        self.host = host
        self.tls = tls
        self.tls_certfile = Path(tls_certfile) if tls_certfile else None
        self.tls_keyfile = Path(tls_keyfile) if tls_keyfile else None

        self._num = server_numeric(numeric)
        self._numnick_mask = self._num + int_to_b64(max_clients, 3)

        self._server: asyncio.AbstractServer | None = None
        self._reader: asyncio.StreamReader | None = None
        self._writer: asyncio.StreamWriter | None = None
        self.port: int | None = None

        self.connected = False
        self.burst_complete = False
        self.peer_name: str | None = None
        self.peer_numeric: str | None = None  # 2-char YY from peer SERVER line
        self.peer_server_line: str | None = None

        self.users: dict[str, dict[str, Any]] = {}
        self.received: list[str] = []
        self.sent: list[str] = []
        self._next_client_num = 0

    @property
    def server_numnick(self) -> str:
        """This hub's 2-char P10 numeric."""
        return self._num

    def _ssl_context(self) -> ssl.SSLContext:
        if not self.tls_certfile or not self.tls_keyfile:
            raise ValueError("tls=True requires tls_certfile and tls_keyfile")
        ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        ctx.minimum_version = ssl.TLSVersion.TLSv1_2
        ctx.load_cert_chain(str(self.tls_certfile), str(self.tls_keyfile))
        # Gnuworld uses SSL_VERIFY_NONE; do not require a client cert.
        ctx.verify_mode = ssl.CERT_NONE
        return ctx

    async def start(self) -> None:
        """Bind an ephemeral port and begin listening (plain or TLS)."""
        ssl_ctx = self._ssl_context() if self.tls else None
        self._server = await asyncio.start_server(
            self._on_connect, self.host, 0, ssl=ssl_ctx
        )
        sockets = self._server.sockets
        assert sockets
        self.port = sockets[0].getsockname()[1]
        logger.debug(
            "FakeHub listening on %s:%d tls=%s",
            self.host,
            self.port,
            self.tls,
        )

    async def _on_connect(
        self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter
    ) -> None:
        if self._reader is not None:
            # Only accept the first connection.
            writer.close()
            await writer.wait_closed()
            return
        self._reader = reader
        self._writer = writer
        self.connected = True
        peer = writer.get_extra_info("peername")
        logger.debug("Accepted connection from %s", peer)

    async def accept_and_handshake(self, timeout: float = 30.0) -> None:
        """Wait for GNUWorld to connect, then complete the P10 handshake.

        Expected order:
          << PASS / SERVER from gnuworld
          >> PASS / SERVER / EB from hub
          << EB / EA from gnuworld (after its empty or module burst)
          >> EA from hub
        """
        deadline = asyncio.get_event_loop().time() + timeout

        # Wait until accept callback fires
        while self._reader is None:
            remaining = deadline - asyncio.get_event_loop().time()
            if remaining <= 0:
                raise TimeoutError("Timed out waiting for gnuworld to connect")
            await asyncio.sleep(0.05)

        # Read PASS
        line = await self._recv(timeout=max(0.1, deadline - asyncio.get_event_loop().time()))
        if p10_token(line) != "PASS":
            raise RuntimeError(f"Expected PASS from gnuworld, got: {line!r}")

        # Read SERVER
        line = await self._recv(timeout=max(0.1, deadline - asyncio.get_event_loop().time()))
        if p10_token(line) != "SERVER":
            raise RuntimeError(f"Expected SERVER from gnuworld, got: {line!r}")
        self.peer_server_line = line
        self._parse_peer_server(line)

        now = int(time.time())
        await self._send(f"PASS :{self.password}")
        flag_field = f"+{self.server_flags}" if self.server_flags else "+"
        await self._send(
            f"SERVER {self.name} 1 {now} {now} J10 {self._numnick_mask} "
            f"{flag_field} :{self.description}"
        )
        # Empty network burst from the hub
        await self._send(f"{self._num} EB")

        # Wait for peer EB then EA
        saw_eb = False
        while True:
            remaining = deadline - asyncio.get_event_loop().time()
            if remaining <= 0:
                raise TimeoutError("Timed out waiting for peer EB/EA")
            line = await self._recv(timeout=remaining)
            tok = p10_token(line)
            if tok == "EB":
                saw_eb = True
                continue
            if tok == "EA" and saw_eb:
                break

        await self._send(f"{self._num} EA")
        self.burst_complete = True
        logger.debug(
            "Handshake complete; peer=%s numeric=%s",
            self.peer_name,
            self.peer_numeric,
        )

    def _parse_peer_server(self, line: str) -> None:
        # SERVER <name> <hop> <boot> <link> J10 <YYXXX> +flags :desc
        parts = strip_msg_tags(line).split()
        # Unprefixed: SERVER name ...
        if parts[0] != "SERVER":
            raise RuntimeError(f"Not a SERVER line: {line!r}")
        self.peer_name = parts[1]
        # Find J10 token then the following numnick mask
        try:
            j10_idx = next(i for i, p in enumerate(parts) if p.startswith("J"))
            mask = parts[j10_idx + 1]
            self.peer_numeric = mask[:2]
        except (StopIteration, IndexError) as exc:
            raise RuntimeError(f"Could not parse numeric from SERVER: {line!r}") from exc

    async def close(self) -> None:
        """Close the client connection and stop listening."""
        if self._writer is not None:
            try:
                if self.burst_complete and self.peer_name:
                    await self._send(f"{self._num} SQ {self.peer_name} 0 :Test done")
            except Exception:
                pass
            try:
                self._writer.close()
                await self._writer.wait_closed()
            except (OSError, ConnectionError):
                pass
        self._writer = None
        self._reader = None
        self.connected = False
        if self._server is not None:
            self._server.close()
            await self._server.wait_closed()
            self._server = None

    async def _send(self, line: str) -> None:
        if not self._writer:
            raise ConnectionError("Not connected")
        logger.debug(">> %s", line)
        self.sent.append(line)
        self._writer.write((line + "\r\n").encode("utf-8"))
        await self._writer.drain()

    async def send_raw(self, line: str) -> None:
        """Send an arbitrary P10 line to gnuworld."""
        await self._send(line)

    async def send_tagged(self, tags: str | dict[str, str | None], line: str) -> None:
        """Send ``line`` with an IRCv3 ``@tag`` prefix.

        ``tags`` is either a raw tag-section (without leading ``@``), e.g.
        ``time=2026-07-25T12:00:00.000Z;+ex/foo=1``, or a dict of key→value
        (``None`` / empty value → key-only tag).
        """
        if isinstance(tags, dict):
            parts = []
            for key, value in tags.items():
                if value is None or value == "":
                    parts.append(key)
                else:
                    parts.append(f"{key}={value}")
            tag_section = ";".join(parts)
        else:
            tag_section = tags.lstrip("@")
        await self._send(f"@{tag_section} {line}")

    async def _recv_raw(self, timeout: float = 10.0) -> str:
        if not self._reader:
            raise ConnectionError("Not connected")
        raw = await asyncio.wait_for(self._reader.readline(), timeout=timeout)
        if not raw:
            raise ConnectionError("Connection closed by peer")
        line = raw.decode("utf-8", errors="replace").rstrip("\r\n")
        logger.debug("<< %s", line)
        self.received.append(line)
        return line

    async def _recv(self, timeout: float = 10.0) -> str:
        """Read one line, auto-answering PINGs; track peer N lines."""
        while True:
            line = await self._recv_raw(timeout=timeout)
            payload = strip_msg_tags(line)
            tokens = payload.split()

            if tokens and tokens[0] == "PING":
                origin = tokens[1].lstrip(":") if len(tokens) > 1 else self.name
                await self._send(f"{self._num} Z {self._num} :{origin}")
                continue
            if len(tokens) >= 2 and tokens[1] == "G":
                # Old: Az G :reason  →  AB Z Az :reason
                # New: Az G !cookie ...
                if len(tokens) == 3 and tokens[2].startswith(":"):
                    await self._send(f"{self._num} Z {tokens[0]} {tokens[2]}")
                else:
                    cookie = tokens[2].lstrip("!") if len(tokens) > 2 else "0"
                    await self._send(f"{self._num} Z {tokens[0]} :{cookie}")
                continue

            if len(tokens) >= 2 and tokens[1] == "N":
                self._parse_nick(payload)

            return line

    def _parse_nick(self, payload: str) -> None:
        # <YY> N <nick> <hop> <ts> <user> <host> <+modes> [..] <ip> <numnick> :<realname>
        parts = payload.split()
        if len(parts) < 10:
            return
        nick = parts[2]
        numnick = parts[-2] if not parts[-1].startswith(":") else parts[-2]
        # Realname is trailing; numnick is the token before it
        for i, p in enumerate(parts):
            if p.startswith(":") and i > 8:
                numnick = parts[i - 1]
                break
        self.users[nick.lower()] = {
            "nick": nick,
            "numnick": numnick,
            "raw": payload,
        }

    def get_user_numnick(self, nick: str) -> str | None:
        info = self.users.get(nick.lower())
        return info["numnick"] if info else None

    async def wait_for(
        self,
        match: str | Predicate,
        timeout: float = 10.0,
        *,
        after: int = 0,
    ) -> str:
        """Wait for a line matching a token string or predicate.

        ``after`` skips ``received[:after]`` so callers can wait for a
        reply that arrives after a specific send.
        """
        if isinstance(match, str):
            token = match

            def pred(line: str) -> bool:
                return p10_token(line) == token

        else:
            pred = match

        deadline = asyncio.get_event_loop().time() + timeout
        start_idx = max(0, after)
        while True:
            for line in self.received[start_idx:]:
                if pred(line):
                    return line
            start_idx = len(self.received)
            remaining = deadline - asyncio.get_event_loop().time()
            if remaining <= 0:
                raise TimeoutError(f"Timed out waiting for {match!r}")
            try:
                await self._recv(timeout=min(remaining, 1.0))
            except asyncio.TimeoutError:
                continue

    async def wait_for_token(
        self, token: str, timeout: float = 10.0, *, after: int = 0
    ) -> str:
        """Wait until a line whose P10 token matches."""
        return await self.wait_for(token, timeout=timeout, after=after)

    async def assert_no_message(self, token: str, timeout: float = 1.0) -> None:
        """Fail if a line with the given token arrives within timeout."""
        try:
            line = await self.wait_for_token(token, timeout=timeout)
        except TimeoutError:
            return
        raise AssertionError(f"Unexpected message with token {token!r}: {line!r}")

    async def introduce_nick(
        self,
        nick: str,
        username: str = "fakeuser",
        host: str = "fake.testnet",
        modes: str = "+i",
        realname: str = "Fake User",
        ip: str = "127.0.0.1",
        hops: int = 1,
        tags: str | dict[str, str | None] | None = None,
    ) -> str:
        """Introduce a nick originating from this hub via a P10 N message.

        Returns the allocated numnick. Optional ``tags`` prefixes the N line
        with an IRCv3 message-tag section (e.g. ``@time=…;+client/tag=…``).
        """
        client_num = self._next_client_num
        self._next_client_num += 1
        numnick = client_numnick(self.numeric, client_num)
        ts = int(time.time())
        ip64 = ipv4_to_b64(ip)
        line = (
            f"{self._num} N {nick} {hops} {ts} {username} {host} {modes} "
            f"{ip64} {numnick} :{realname}"
        )
        if tags is not None:
            await self.send_tagged(tags, line)
        else:
            await self._send(line)
        self.users[nick.lower()] = {
            "nick": nick,
            "numnick": numnick,
            "user": username,
            "host": host,
            "modes": modes,
            "realname": realname,
            "ip": ip,
        }
        return numnick

    async def burst_channel(
        self,
        channel: str,
        members: list[str] | None = None,
        modes: str = "+tn",
        ts: int | None = None,
    ) -> None:
        """Burst a channel via a P10 B message.

        ``members`` is a list of numnicks, optionally with modes like ``ABAAA:o``.
        If omitted, uses all currently introduced hub users as plain members.
        """
        if ts is None:
            ts = int(time.time())
        if members is None:
            members = [u["numnick"] for u in self.users.values()]
        member_field = ",".join(members) if members else ""
        # Format: <YY> B <chan> <ts> +modes [modeargs] members
        if member_field:
            await self._send(f"{self._num} B {channel} {ts} {modes} {member_field}")
        else:
            await self._send(f"{self._num} B {channel} {ts} {modes}")

    async def send_xquery(
        self,
        routing: str,
        message: str,
        target: str | None = None,
        tags: str | dict[str, str | None] | None = None,
    ) -> None:
        """Send an XQUERY to gnuworld.

        Wire: ``[@tags ]<hubYY> XQ <targetYY> <routing> :<message>``
        ``target`` defaults to the peer's server numeric from handshake.
        """
        if target is None:
            if not self.peer_numeric:
                raise RuntimeError("No peer numeric; handshake not complete")
            target = self.peer_numeric
        line = f"{self._num} XQ {target} {routing} :{message}"
        if tags is not None:
            await self.send_tagged(tags, line)
        else:
            await self._send(line)

    async def send_privmsg(
        self,
        from_numnick: str,
        target: str,
        text: str,
        tags: str | dict[str, str | None] | None = None,
    ) -> None:
        line = f"{from_numnick} P {target} :{text}"
        if tags is not None:
            await self.send_tagged(tags, line)
        else:
            await self._send(line)

    async def send_xreply(
        self,
        routing: str,
        reply: str,
        target: str | None = None,
    ) -> None:
        """Send an XREPLY.

        Wire: ``<hubYY> XR <targetYY> <routing> :<reply>``
        """
        if target is None:
            if not self.peer_numeric:
                raise RuntimeError("No peer numeric; handshake not complete")
            target = self.peer_numeric
        await self._send(f"{self._num} XR {target} {routing} :{reply}")

    async def send_notice(self, from_numnick: str, target: str, text: str) -> None:
        await self._send(f"{from_numnick} O {target} :{text}")

    async def introduce_leaf(
        self,
        name: str,
        numeric: int,
        *,
        hop: int = 2,
        flags: str = "",
        description: str = "Downstream test server",
        timestamp: int | None = None,
        protocol: str = "J10",
    ) -> str:
        """Introduce a remote leaf behind this hub (P10 ``S``).

        ``flags`` are letters without ``+`` (e.g. ``\"z\"`` for TLS, ``\"hz\"``
        for hub+TLS). Returns the leaf's 2-char P10 numeric.
        """
        ts = timestamp if timestamp is not None else int(time.time())
        down_num = server_numeric(numeric)
        down_mask = down_num + int_to_b64(self.max_clients, 3)
        flag_field = f"+{flags}" if flags else "+"
        await self._send(
            f"{self._num} S {name} {hop} 0 {ts} {protocol} {down_mask} "
            f"{flag_field} :{description}"
        )
        return down_num

    async def end_burst(self, server_yy: str | None = None) -> None:
        """Send End of Burst for ``server_yy`` (defaults to this hub)."""
        yy = server_yy or self._num
        await self._send(f"{yy} EB")

    async def introduce_leaf_nick(
        self,
        leaf_yy: str,
        leaf_numeric: int,
        nick: str,
        *,
        client_num: int = 1,
        username: str | None = None,
        host: str = "leaf.testnet",
        modes: str = "+i",
        realname: str = "Leaf User",
        ip: str = "127.0.0.1",
        hops: int = 1,
    ) -> str:
        """Introduce a nick homed on a previously announced leaf server."""
        ts = int(time.time())
        user = username or nick.lower()
        numnick = client_numnick(leaf_numeric, client_num)
        ip64 = ipv4_to_b64(ip)
        await self._send(
            f"{leaf_yy} N {nick} {hops} {ts} {user} {host} {modes} "
            f"{ip64} {numnick} :{realname}"
        )
        self.users[nick.lower()] = {
            "nick": nick,
            "numnick": numnick,
            "user": user,
            "host": host,
            "modes": modes,
            "realname": realname,
            "ip": ip,
            "server": leaf_yy,
        }
        return numnick

    async def send_account(
        self,
        target_numnick: str,
        account: str,
        *,
        acc_id: int | None = None,
        acc_flags: int | None = None,
        from_yy: str | None = None,
    ) -> None:
        """Set a client's account via P10 ``AC``.

        Wire: ``<YY> AC <target> <account> [<id> [<flags>]]``
        """
        yy = from_yy or self._num
        parts = f"{yy} AC {target_numnick} {account}"
        if acc_id is not None:
            parts += f" {acc_id}"
            if acc_flags is not None:
                parts += f" {acc_flags}"
        await self._send(parts)

    async def drain_messages(self, timeout: float = 0.5) -> None:
        """Read and process pending messages until quiet."""
        while True:
            try:
                await self._recv(timeout=timeout)
            except (asyncio.TimeoutError, TimeoutError):
                break
