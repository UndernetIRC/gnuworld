## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.7 2003/09/02 00:54:58 jeekay Exp $"

lib_LTLIBRARIES = libclientExample.la
libclientExample_la_SOURCES = clientExample.cc
libclientExample_la_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
libclientExample_la_LDFLAGS = -module -export-dynamic

EXTRA_DIST = clientExample.h
