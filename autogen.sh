#!/bin/sh

# Regenerate auto-tools stuff...

aclocal
autoconf
libtoolize --automake --copy --ltdl
autoheader
automake -a -c

