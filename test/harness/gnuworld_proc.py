"""Spawn and supervise GNUWorld (Docker) for integration tests.

Postgres and the gnuworld image run under docker compose. The FakeHub
listens on the host; gnuworld uses host networking to reach it at
``127.0.0.1``.

Stdout from ``docker compose run -T gnuworld`` (gnuworld ``-c``) is
piped into this process so tests can call ``wait_for_stdout`` exactly
as they would with a local binary.
"""

from __future__ import annotations

import asyncio
import logging
import os
import re
import signal
import subprocess
import time
import uuid
from pathlib import Path

logger = logging.getLogger("gnuworld_proc")

HARNESS_DIR = Path(__file__).resolve().parent
REPO_ROOT = HARNESS_DIR.parent.parent
COMPOSE_FILE = HARNESS_DIR / "docker-compose.yml"
CONF_TEMPLATE = HARNESS_DIR / "data" / "gnuworld.harness.conf.in"
CCONTROL_CONF_TEMPLATE = HARNESS_DIR / "data" / "ccontrol.harness.conf.in"
DEBUG_CONF_TEMPLATE = HARNESS_DIR / "data" / "debug.harness.conf.in"
TLS_DIR = HARNESS_DIR / "data" / "tls"
RUN_DIR = HARNESS_DIR / "run"

# Paths inside the gnuworld container / host-network process
CONTAINER_CONF_DIR = "/etc/gnuworld"
CONTAINER_COMMAND_MAP = "/opt/gnuworld/share/gnuworld/server_command_map"
CONTAINER_LIBDIR = "/opt/gnuworld/lib"
# host networking: FakeHub and published Postgres are on the host loopback
CONTAINER_UPLINK = "127.0.0.1"

# Postgres via published host port (gnuworld uses network_mode: host)
DEFAULT_SQL_HOST = "127.0.0.1"
DEFAULT_SQL_PORT = "5433"
DEFAULT_SQL_DB = "ccontrol"
DEFAULT_SQL_USER = "gnuworld"
DEFAULT_SQL_PASS = "gnuworld"


def _compose_cmd(*args: str) -> list[str]:
    return ["docker", "compose", "-f", str(COMPOSE_FILE), *args]


class DockerStack:
    """Session-scoped helper: build image, start Postgres, tear down."""

    def __init__(self) -> None:
        self.started = False

    def up(self) -> None:
        RUN_DIR.mkdir(parents=True, exist_ok=True)
        logger.info("Building gnuworld image and starting Postgres...")
        subprocess.run(
            _compose_cmd("build", "gnuworld"),
            cwd=str(HARNESS_DIR),
            check=True,
        )
        subprocess.run(
            _compose_cmd("up", "-d", "postgres"),
            cwd=str(HARNESS_DIR),
            check=True,
        )
        self._wait_healthy("postgres", timeout=60.0)
        self.started = True

    def down(self) -> None:
        subprocess.run(
            _compose_cmd("down", "-v", "--remove-orphans"),
            cwd=str(HARNESS_DIR),
            check=False,
        )
        self.started = False

    def _wait_healthy(self, service: str, timeout: float) -> None:
        deadline = time.time() + timeout
        while time.time() < deadline:
            ready = subprocess.run(
                _compose_cmd(
                    "exec",
                    "-T",
                    "postgres",
                    "pg_isready",
                    "-U",
                    DEFAULT_SQL_USER,
                    "-d",
                    DEFAULT_SQL_DB,
                ),
                cwd=str(HARNESS_DIR),
                capture_output=True,
                check=False,
            )
            if ready.returncode == 0:
                logger.info("Postgres is ready")
                return
            time.sleep(1.0)
        raise TimeoutError(f"Timed out waiting for {service} to become ready")


class GnuworldProc:
    """Runs gnuworld via ``docker compose run`` and captures stdout."""

    def __init__(self, conf_dir: Path, container_name: str | None = None):
        """``conf_dir`` is a host directory bind-mounted at /etc/gnuworld.

        It must contain ``GNUWorld.conf`` (and optionally module confs).
        """
        self.conf_dir = Path(conf_dir)
        self.container_name = container_name or f"gw-test-{uuid.uuid4().hex[:10]}"
        self.proc: asyncio.subprocess.Process | None = None
        self.stdout_lines: list[str] = []
        self._reader_task: asyncio.Task | None = None
        self._line_event = asyncio.Event()

    @staticmethod
    def write_config(
        path: Path,
        *,
        uplink: str = CONTAINER_UPLINK,
        port: int,
        password: str = "testpass",
        name: str = "services.testnet",
        numeric: int = 51,
        command_map: str = CONTAINER_COMMAND_MAP,
        libdir: str = CONTAINER_LIBDIR,
        module_lines: str = "",
        tls: bool = False,
        tls_key_file: str | None = None,
        tls_cert_file: str | None = None,
    ) -> Path:
        text = CONF_TEMPLATE.read_text(encoding="utf-8")
        if tls:
            key = tls_key_file or f"{CONTAINER_CONF_DIR}/gnuworld.key"
            cert = tls_cert_file or f"{CONTAINER_CONF_DIR}/gnuworld.crt"
            tls_files = f"tlsKeyFile = {key}\ntlsCertFile = {cert}\n"
            tls_val = "yes"
        else:
            tls_files = ""
            tls_val = "no"
        replacements = {
            "@UPLINK@": uplink,
            "@PORT@": str(port),
            "@PASSWORD@": password,
            "@NAME@": name,
            "@NUMERIC@": str(numeric),
            "@COMMAND_MAP@": command_map,
            "@LIBDIR@": libdir,
            "@MODULE_LINES@": module_lines,
            "@TLS@": tls_val,
            "@TLS_FILES@": tls_files,
        }
        for key, value in replacements.items():
            text = text.replace(key, value)
        path.write_text(text, encoding="utf-8")
        return path

    @staticmethod
    def install_tls_certs(conf_dir: Path) -> tuple[Path, Path, Path, Path]:
        """Copy harness TLS material into the bind-mounted conf dir.

        Returns (hub_crt, hub_key, gw_crt, gw_key) host paths.
        """
        import shutil

        for name in ("hub.crt", "hub.key", "gnuworld.crt", "gnuworld.key"):
            shutil.copy2(TLS_DIR / name, conf_dir / name)
        return (
            conf_dir / "hub.crt",
            conf_dir / "hub.key",
            conf_dir / "gnuworld.crt",
            conf_dir / "gnuworld.key",
        )

    @staticmethod
    def write_ccontrol_config(
        path: Path,
        *,
        sql_host: str = DEFAULT_SQL_HOST,
        sql_port: str = DEFAULT_SQL_PORT,
        sql_db: str = DEFAULT_SQL_DB,
        sql_user: str = DEFAULT_SQL_USER,
        sql_pass: str = DEFAULT_SQL_PASS,
    ) -> Path:
        text = CCONTROL_CONF_TEMPLATE.read_text(encoding="utf-8")
        replacements = {
            "@SQL_HOST@": os.environ.get("CCONTROL_SQL_HOST", sql_host),
            "@SQL_PORT@": os.environ.get("CCONTROL_SQL_PORT", sql_port),
            "@SQL_DB@": os.environ.get("CCONTROL_SQL_DB", sql_db),
            "@SQL_USER@": os.environ.get("CCONTROL_SQL_USER", sql_user),
            "@SQL_PASS@": os.environ.get("CCONTROL_SQL_PASS", sql_pass),
        }
        for key, value in replacements.items():
            text = text.replace(key, value)
        path.write_text(text, encoding="utf-8")
        return path

    @staticmethod
    def write_debug_config(
        path: Path,
        *,
        permit_user: str = "MrIron",
    ) -> Path:
        text = DEBUG_CONF_TEMPLATE.read_text(encoding="utf-8")
        path.write_text(
            text.replace("@PERMIT_USER@", permit_user),
            encoding="utf-8",
        )
        return path

    async def start(self) -> None:
        if not (self.conf_dir / "GNUWorld.conf").is_file():
            raise FileNotFoundError(f"Missing {self.conf_dir / 'GNUWorld.conf'}")

        # Bind-mount this test's conf dir over /etc/gnuworld for the one-off run.
        cmd = _compose_cmd(
            "run",
            "--rm",
            "-T",
            "--name",
            self.container_name,
            "-v",
            f"{self.conf_dir.resolve()}:{CONTAINER_CONF_DIR}:ro",
            "gnuworld",
        )
        logger.debug("Starting: %s", " ".join(cmd))
        self.proc = await asyncio.create_subprocess_exec(
            *cmd,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.STDOUT,
            cwd=str(HARNESS_DIR),
            start_new_session=True,
        )
        self._reader_task = asyncio.create_task(self._read_stdout())

    async def _read_stdout(self) -> None:
        assert self.proc and self.proc.stdout
        while True:
            raw = await self.proc.stdout.readline()
            if not raw:
                break
            line = raw.decode("utf-8", errors="replace").rstrip("\r\n")
            logger.debug("GW| %s", line)
            self.stdout_lines.append(line)
            self._line_event.set()

    async def wait_for_stdout(
        self,
        pattern: str | re.Pattern[str],
        timeout: float = 30.0,
    ) -> str:
        if isinstance(pattern, str):
            regex = re.compile(re.escape(pattern))
        else:
            regex = pattern

        deadline = asyncio.get_event_loop().time() + timeout
        start_idx = 0
        while True:
            for line in self.stdout_lines[start_idx:]:
                if regex.search(line):
                    return line
            start_idx = len(self.stdout_lines)
            remaining = deadline - asyncio.get_event_loop().time()
            if remaining <= 0:
                recent = "\n".join(self.stdout_lines[-40:])
                raise TimeoutError(
                    f"Timed out waiting for stdout matching {pattern!r}. "
                    f"Recent output:\n{recent}"
                )
            self._line_event.clear()
            try:
                await asyncio.wait_for(self._line_event.wait(), timeout=min(remaining, 1.0))
            except asyncio.TimeoutError:
                if self.proc and self.proc.returncode is not None:
                    recent = "\n".join(self.stdout_lines[-40:])
                    raise RuntimeError(
                        f"gnuworld exited with {self.proc.returncode} while waiting "
                        f"for {pattern!r}. Recent output:\n{recent}"
                    )
                continue

    async def terminate(self, grace: float = 5.0) -> None:
        if self.proc is None:
            return
        if self.proc.returncode is not None:
            if self._reader_task:
                await self._reader_task
            return

        # Prefer docker stop so gnuworld can shut down cleanly
        await asyncio.to_thread(
            subprocess.run,
            ["docker", "stop", "-t", "3", self.container_name],
            capture_output=True,
            check=False,
        )

        try:
            await asyncio.wait_for(self.proc.wait(), timeout=grace)
        except asyncio.TimeoutError:
            try:
                os.killpg(self.proc.pid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            await self.proc.wait()

        if self._reader_task:
            try:
                await asyncio.wait_for(self._reader_task, timeout=2.0)
            except asyncio.TimeoutError:
                self._reader_task.cancel()
