# Runtime image for integration tests: host-built binaries + libs.
FROM debian:trixie-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        libltdl7 \
        libpq5 \
        libssl3 \
        libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/gnuworld

# Prebuilt artifacts from the repo (build on the host first).
COPY bin/gnuworld bin/gnuworld
COPY lib/ lib/
COPY share/gnuworld/ share/gnuworld/
COPY mod.ccontrol/migrations/ migrations/ccontrol/

ENV LD_LIBRARY_PATH=/opt/gnuworld/lib
WORKDIR /opt/gnuworld/bin

ENTRYPOINT ["./gnuworld"]
CMD ["-c", "-f", "/etc/gnuworld/GNUWorld.conf", "-L", "-D"]
