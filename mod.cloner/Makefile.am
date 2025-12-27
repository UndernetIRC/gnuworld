## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.8 2003/09/02 00:54:58 jeekay Exp $"

lib_LTLIBRARIES += libcloner.la
libcloner_la_SOURCES = mod.cloner/cloner.cc
libcloner_la_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier
libcloner_la_LDFLAGS = -module -export-dynamic

EXTRA_DIST += mod.cloner/cloner.h
