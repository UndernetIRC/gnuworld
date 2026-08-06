"""Smoke tests: P10 link-up and basic FakeHub helpers."""

from __future__ import annotations

import pytest

from p10 import p10_token


@pytest.mark.asyncio
async def test_link_handshake(linked):
    hub, proc = linked

    assert hub.burst_complete
    assert hub.peer_name == "services.testnet"
    assert hub.peer_numeric is not None
    assert hub.peer_server_line is not None
    assert hub.peer_numeric in hub.peer_server_line

    # GNUWorld must have completed its burst toward the hub
    assert any(p10_token(line) == "EB" for line in hub.received)
    assert any(p10_token(line) == "EA" for line in hub.received)

    await proc.wait_for_stdout("Connected")


@pytest.mark.asyncio
async def test_introduce_nick_and_burst_channel(linked):
    hub, proc = linked

    numnick = await hub.introduce_nick(
        "alice",
        username="ali",
        host="alice.testnet",
        realname="Alice Example",
    )
    assert len(numnick) == 5
    assert hub.get_user_numnick("alice") == numnick

    await hub.burst_channel("#test", members=[f"{numnick}:o"])
    assert any(
        p10_token(line) == "B" and "#test" in line for line in hub.sent
    )

    # GNUWorld should still be alive after accepting N/B
    assert proc.proc is not None
    assert proc.proc.returncode is None


@pytest.mark.asyncio
async def test_send_xquery_helper(linked):
    hub, _proc = linked

    await hub.send_xquery(
        routing="iauth:1",
        message="CHECK alice ali 127.0.0.1 alice.testnet :Alice Example",
    )
    assert any(p10_token(line) == "XQ" for line in hub.sent)
    xq = next(line for line in hub.sent if p10_token(line) == "XQ")
    assert hub.peer_numeric in xq
    assert "CHECK alice" in xq
