## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.8 2003/09/14 16:35:54 dan_karrels Exp $"

lib_LTLIBRARIES += libclientExample.la
libclientExample_la_SOURCES = mod.clientExample/clientExample.cc
libclientExample_la_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier
libclientExample_la_LDFLAGS = -module -export-dynamic

EXTRA_DIST += mod.clientExample/clientExample.h
