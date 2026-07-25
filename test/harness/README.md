# GNUWorld integration tests (`test/harness`)

Stand-alone pytest harness that:

1. Listens as a fake ircu P10 hub **on the host**
2. Runs **Postgres + gnuworld in Docker Compose**
3. Lets tests assert on S2S lines the hub receives and on **gnuworld container stdout**

Gnuworld is started with `docker compose run --rm -T` so its `-c` verbose
output is piped into the test process (`wait_for_stdout`).

C++ unit/load tools remain in `test/` (automake `test_*` binaries). This
directory is the Python integration suite only.

## Prerequisites

- Docker + Docker Compose
- A host build of gnuworld (`bin/gnuworld`, `lib/`, `share/gnuworld/`) — the
  image copies these artifacts (it does not compile inside Docker)

## Setup

```bash
cd test/harness
python3 -m venv .venv
source .venv/bin/activate
pip install pytest pytest-asyncio pytest-timeout
```

## Run

```bash
cd test/harness
pytest -v
```

The session fixture builds the image, starts Postgres, and tears the stack
down at the end. First run is slower due to `docker compose build`.

## Writing tests

```python
async def test_example(linked):
    hub, proc = linked
    await proc.wait_for_stdout("Connected")
    line = await hub.wait_for_token("EB")

async def test_check(ccontrol_linked):
    hub, proc = ccontrol_linked
    await hub.send_xquery(routing="iauth:1", message="CHECK ...")
```

## Layout

| Path | Role |
|------|------|
| `docker-compose.yml` | Postgres + gnuworld image |
| `docker/Dockerfile` | Runtime image from host-built binaries |
| `docker/initdb/` | ccontrol schema for Postgres |
| `fake_hub.py` | Listening fake P10 hub |
| `gnuworld_proc.py` | `compose run` + stdout capture |
| `data/*.conf.in` | Config templates |

Gnuworld reaches FakeHub at `127.0.0.1:<port>` (container uses host networking).
ccontrol reaches Postgres at `127.0.0.1:5433` (published compose port).
