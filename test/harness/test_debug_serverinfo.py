"""mod.debug SERVERINFO — TLS flags on leaf (+z) and TLS uplink transport."""

from __future__ import annotations

import pytest

from p10 import p10_token

PERMIT_ACCOUNT = "MrIron"
LEAF_NAME = "leaf-tls.testnet"
LEAF_NUMERIC = 2


async def _serverinfo_flags(
    hub,
    *,
    target: str,
    expect_tls: bool,
) -> str:
    """Introduce an authed nick, PRIVMSG SERVERINFO, return the Flags Notice."""
    assert hub.peer_name, "gnuworld peer name missing after handshake"
    numnick = await hub.introduce_nick("authed", username="authed")
    await hub.send_account(numnick, PERMIT_ACCOUNT)

    after = len(hub.received)
    await hub.send_privmsg(
        numnick,
        f"debug@{hub.peer_name}",
        f"SERVERINFO {target}",
    )

    want = "tls=yes" if expect_tls else "tls=no"

    def is_flags_notice(line: str) -> bool:
        if p10_token(line) != "O":
            return False
        return numnick in line and want in line and "Flags:" in line

    return await hub.wait_for(is_flags_notice, timeout=10.0, after=after)


@pytest.mark.asyncio
async def test_serverinfo_leaf_with_tls_flag(debug_linked):
    """Spawn a +z leaf, PRIVMSG stealth debug as an authed nick, expect tls=yes."""
    hub, proc = debug_linked

    leaf_yy = await hub.introduce_leaf(LEAF_NAME, LEAF_NUMERIC, flags="z")
    await hub.end_burst(leaf_yy)

    numnick = await hub.introduce_leaf_nick(
        leaf_yy,
        LEAF_NUMERIC,
        "authed",
        username="authed",
        host="user.leaf.testnet",
    )
    await hub.send_account(numnick, PERMIT_ACCOUNT)

    after = len(hub.received)
    await hub.send_privmsg(
        numnick,
        f"debug@{hub.peer_name}",
        f"SERVERINFO {LEAF_NAME}",
    )

    def is_flags_notice(line: str) -> bool:
        if p10_token(line) != "O":
            return False
        return numnick in line and "tls=yes" in line and "Flags:" in line

    flags_line = await hub.wait_for(is_flags_notice, timeout=10.0, after=after)
    assert "tls=yes" in flags_line
    assert any(
        p10_token(line) == "O" and LEAF_NAME in line for line in hub.received[after:]
    ), "expected a SERVERINFO Notice naming the leaf"

    assert proc.proc is not None
    assert proc.proc.returncode is None


@pytest.mark.asyncio
async def test_serverinfo_uplink_tls_from_transport(debug_linked_tls):
    """TLS uplink (no +z on SERVER) must still report tls=yes via transport."""
    hub, proc = debug_linked_tls
    assert hub.tls

    flags_line = await _serverinfo_flags(hub, target=hub.name, expect_tls=True)
    assert "tls=yes" in flags_line

    assert proc.proc is not None
    assert proc.proc.returncode is None


@pytest.mark.asyncio
async def test_serverinfo_uplink_plain_is_not_tls(debug_linked):
    """Plain uplink without +z must report tls=no."""
    hub, proc = debug_linked
    assert not hub.tls

    flags_line = await _serverinfo_flags(hub, target=hub.name, expect_tls=False)
    assert "tls=no" in flags_line

    assert proc.proc is not None
    assert proc.proc.returncode is None
