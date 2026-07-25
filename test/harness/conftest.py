"""Pytest fixtures for the GNUWorld fake-hub harness (Dockerized GW + Postgres)."""

from __future__ import annotations

from pathlib import Path

import pytest
import pytest_asyncio

from fake_hub import FakeHub
from gnuworld_proc import (
    CONTAINER_CONF_DIR,
    CONTAINER_UPLINK,
    DockerStack,
    GnuworldProc,
)


@pytest.fixture(scope="session")
def docker_stack():
    """Build the gnuworld image and start Postgres for the test session."""
    stack = DockerStack()
    stack.up()
    try:
        yield stack
    finally:
        stack.down()


@pytest_asyncio.fixture
async def fake_hub():
    """Listen on loopback; gnuworld uses host networking to reach it."""
    hub = FakeHub(host="127.0.0.1")
    await hub.start()
    try:
        yield hub
    finally:
        await hub.close()


def _prepare_conf_dir(tmp_path: Path) -> Path:
    """Host dir bind-mounted at /etc/gnuworld inside the container."""
    conf_dir = tmp_path / "etc-gnuworld"
    conf_dir.mkdir()
    return conf_dir


@pytest_asyncio.fixture
async def gnuworld(docker_stack, fake_hub, tmp_path):
    """Start Dockerized gnuworld (no modules) linked to FakeHub."""
    hub = fake_hub
    conf_dir = _prepare_conf_dir(tmp_path)
    GnuworldProc.write_config(
        conf_dir / "GNUWorld.conf",
        uplink=CONTAINER_UPLINK,
        port=hub.port,
        password=hub.password,
    )

    proc = GnuworldProc(conf_dir=conf_dir)
    await proc.start()
    try:
        await hub.accept_and_handshake(timeout=45.0)
        await proc.wait_for_stdout("Connected", timeout=30.0)
        yield hub, proc
    finally:
        await proc.terminate()


@pytest_asyncio.fixture
async def linked(gnuworld):
    yield gnuworld


@pytest_asyncio.fixture
async def ccontrol_linked(docker_stack, fake_hub, tmp_path):
    """Dockerized gnuworld with libccontrol against compose Postgres."""
    hub = fake_hub
    conf_dir = _prepare_conf_dir(tmp_path)
    GnuworldProc.write_ccontrol_config(conf_dir / "ccontrol.conf")
    GnuworldProc.write_config(
        conf_dir / "GNUWorld.conf",
        uplink=CONTAINER_UPLINK,
        port=hub.port,
        password=hub.password,
        module_lines=f"module = libccontrol.la {CONTAINER_CONF_DIR}/ccontrol.conf",
    )

    proc = GnuworldProc(conf_dir=conf_dir)
    await proc.start()
    try:
        await hub.accept_and_handshake(timeout=90.0)
        await proc.wait_for_stdout("Connected", timeout=60.0)
        assert hub.get_user_numnick("euworld") or any(
            " N euworld " in line for line in hub.received
        ), "ccontrol did not burst euworld (module/DB load failed?)"
        yield hub, proc
    finally:
        await proc.terminate()


@pytest_asyncio.fixture
async def debug_linked(docker_stack, fake_hub, tmp_path):
    """Dockerized gnuworld with stealth mod.debug (no DB required)."""
    hub = fake_hub
    conf_dir = _prepare_conf_dir(tmp_path)
    GnuworldProc.write_debug_config(conf_dir / "debug.conf")
    GnuworldProc.write_config(
        conf_dir / "GNUWorld.conf",
        uplink=CONTAINER_UPLINK,
        port=hub.port,
        password=hub.password,
        module_lines=f"module = libdebug.la {CONTAINER_CONF_DIR}/debug.conf",
    )

    proc = GnuworldProc(conf_dir=conf_dir)
    await proc.start()
    try:
        await hub.accept_and_handshake(timeout=90.0)
        await proc.wait_for_stdout("Connected", timeout=60.0)
        await proc.wait_for_stdout("Loaded stealth client, nickname: debug", timeout=30.0)
        yield hub, proc
    finally:
        await proc.terminate()


@pytest_asyncio.fixture
async def debug_linked_tls(docker_stack, tmp_path):
    """Stealth mod.debug over a TLS uplink (hub SERVER flags omit +z)."""
    conf_dir = _prepare_conf_dir(tmp_path)
    hub_crt, hub_key, _gw_crt, _gw_key = GnuworldProc.install_tls_certs(conf_dir)

    hub = FakeHub(
        host="127.0.0.1",
        tls=True,
        tls_certfile=hub_crt,
        tls_keyfile=hub_key,
        # Intentionally no 'z' — tls=yes must come from the transport.
        server_flags="hs",
    )
    await hub.start()

    GnuworldProc.write_debug_config(conf_dir / "debug.conf")
    GnuworldProc.write_config(
        conf_dir / "GNUWorld.conf",
        uplink=CONTAINER_UPLINK,
        port=hub.port,
        password=hub.password,
        module_lines=f"module = libdebug.la {CONTAINER_CONF_DIR}/debug.conf",
        tls=True,
    )

    proc = GnuworldProc(conf_dir=conf_dir)
    await proc.start()
    try:
        await hub.accept_and_handshake(timeout=90.0)
        await proc.wait_for_stdout("Connected", timeout=60.0)
        await proc.wait_for_stdout("TLS handshake completed successfully", timeout=30.0)
        await proc.wait_for_stdout("Loaded stealth client, nickname: debug", timeout=30.0)
        yield hub, proc
    finally:
        await proc.terminate()
        await hub.close()
