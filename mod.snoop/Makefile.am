## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.1 2003/12/04 00:44:24 dan_karrels Exp $"

lib_LTLIBRARIES = libsnoop.la
libsnoop_la_SOURCES = snoop.cc
libsnoop_la_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld
libsnoop_la_LDFLAGS = -module -export-dynamic

EXTRA_DIST = snoop.h
