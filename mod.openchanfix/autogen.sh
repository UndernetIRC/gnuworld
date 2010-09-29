#! /bin/sh

aclocal -I .
autoheader -Wall
automake -a --gnu Makefile
autoconf -Wall

