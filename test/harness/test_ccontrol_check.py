"""ccontrol iauth CHECK XQ — legacy and account-aware syntax."""

from __future__ import annotations

import pytest

from p10 import p10_token


async def _check_and_expect_ok(hub, routing: str, check_body: str) -> str:
    """Send CHECK via XQ and wait for a matching XR :OK."""
    after = len(hub.received)
    await hub.send_xquery(routing=routing, message=check_body)

    def is_our_ok(line: str) -> bool:
        if p10_token(line) != "XR":
            return False
        return routing in line and ":OK" in line

    return await hub.wait_for(is_our_ok, timeout=10.0, after=after)


@pytest.mark.asyncio
async def test_ccontrol_check_legacy_syntax(ccontrol_linked):
    """Old iauth CHECK: nick user ip host :fullname (no account field)."""
    hub, _proc = ccontrol_linked

    xr = await _check_and_expect_ok(
        hub,
        routing="iauth:legacy1",
        check_body="CHECK alice ali 192.0.2.10 alice.testnet :Alice Example",
    )
    assert "iauth:legacy1" in xr
    assert xr.endswith(":OK") or " :OK" in xr


@pytest.mark.asyncio
async def test_ccontrol_check_with_account(ccontrol_linked):
    """New iauth CHECK: nick user ip host account :fullname."""
    hub, _proc = ccontrol_linked

    xr = await _check_and_expect_ok(
        hub,
        routing="iauth:acct1",
        check_body="CHECK bob bobby 192.0.2.11 bob.testnet bobaccount :Bob Example",
    )
    assert "iauth:acct1" in xr


@pytest.mark.asyncio
async def test_ccontrol_check_with_star_account(ccontrol_linked):
    """New iauth CHECK with explicit no-account marker '*'."""
    hub, _proc = ccontrol_linked

    xr = await _check_and_expect_ok(
        hub,
        routing="iauth:star1",
        check_body="CHECK carol car 192.0.2.12 carol.testnet * :Carol Example",
    )
    assert "iauth:star1" in xr
