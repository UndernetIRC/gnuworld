"""IRCv3 message-tags on S2S (@time and @+client-tags).

Gnuworld strips a leading @tag-section before dispatching P10 handlers, so
tagged client events and tagged XQUERY must still work. Outbound service
RPC (XR) must remain untagged — positional parsers on the hub/iauth side
break if @time= is prepended (see ircu msgtags compat suite).
"""

from __future__ import annotations

import pytest

from p10 import p10_token

SERVER_TIME = "2026-07-25T12:00:00.000Z"
# Client-only tag key (IRCv3 +prefix); gnuworld stores it as an ordinary key.
CLIENT_TAG = {"+example.com/foo": "bar"}


def _time_and_client_tags() -> dict[str, str | None]:
    return {"time": SERVER_TIME, **CLIENT_TAG}


async def _still_alive(hub, proc) -> None:
    await hub.drain_messages(timeout=0.3)
    assert proc.proc is not None
    assert proc.proc.returncode is None


@pytest.mark.asyncio
async def test_tagged_nick_with_time_and_client_tag(linked):
    """@time + @+client-tag on N must be stripped; nick still introduced."""
    hub, proc = linked
    numnick = await hub.introduce_nick(
        "tagnick",
        tags=_time_and_client_tags(),
    )
    assert numnick
    assert any(line.lstrip().startswith("@") and " N tagnick " in line for line in hub.sent)
    await _still_alive(hub, proc)


@pytest.mark.asyncio
async def test_tagged_privmsg_with_time_and_client_tag(linked):
    """Tagged channel PRIVMSG must not crash or break the link."""
    hub, proc = linked
    numnick = await hub.introduce_nick("tagger")
    await hub.burst_channel("#msgtags", members=[numnick])
    await hub.send_privmsg(
        numnick,
        "#msgtags",
        "hello tagged",
        tags=_time_and_client_tags(),
    )
    assert any(
        line.lstrip().startswith("@") and " P #msgtags :" in line for line in hub.sent
    )
    await _still_alive(hub, proc)


@pytest.mark.asyncio
async def test_tags_only_line_ignored(linked):
    """A lone @tag-section with no command is dropped silently."""
    hub, proc = linked
    await hub.send_raw(f"@time={SERVER_TIME}")
    await _still_alive(hub, proc)


@pytest.mark.asyncio
async def test_tagged_xq_check_still_ok_and_xr_untagged(ccontrol_linked):
    """Tagged XQ CHECK is handled; gnuworld's XR must not carry @tags."""
    hub, _proc = ccontrol_linked
    routing = "iauth:tagged1"
    after = len(hub.received)

    await hub.send_xquery(
        routing=routing,
        message="CHECK taggeduser tu 192.0.2.50 tagged.testnet :Tagged User",
        tags=_time_and_client_tags(),
    )
    assert any(
        line.lstrip().startswith("@") and " XQ " in line and routing in line
        for line in hub.sent
    )

    def is_our_ok(line: str) -> bool:
        if p10_token(line) != "XR":
            return False
        return routing in line and ":OK" in line

    xr = await hub.wait_for(is_our_ok, timeout=10.0, after=after)

    # Raw wire: service RPC must stay untagged (positional S2S parsers).
    assert not xr.lstrip().startswith("@"), f"XR unexpectedly tagged: {xr!r}"
    parts = xr.split()
    assert parts[1] == "XR"
    assert routing in xr
    assert ":OK" in xr
