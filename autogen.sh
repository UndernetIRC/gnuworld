#!/bin/sh
# Bootstrap the GNUWorld autotools build system.
#
# Requires: autoconf, automake, libtool (libtoolize).
# Does not require pkg-config/pkgconf at bootstrap time (pkg.m4 is vendored
# under m4/); pkg-config is only needed when running ./configure if you want
# optional libraries discovered that way (libcurl, prometheus-cpp).

set -e

cd "$(dirname "$0")"

missing=
for tool in autoconf automake libtoolize autoreconf; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		missing="$missing $tool"
	fi
done
if [ -n "$missing" ]; then
	echo "autogen.sh: missing required tool(s):$missing" >&2
	echo "Install autoconf, automake, and libtool, then re-run." >&2
	exit 1
fi

# Ensure our macro directory exists for libtoolize / aclocal.
mkdir -p m4

echo "Running autoreconf -fiv ..."
autoreconf -fiv

echo
echo "Bootstrap complete. Next:"
echo "  ./configure --enable-modules=..."
echo "  make"
