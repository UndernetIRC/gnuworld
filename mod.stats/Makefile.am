## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.9 2003/09/02 00:54:58 jeekay Exp $"

lib_LTLIBRARIES += libstats.la
libstats_la_SOURCES = mod.stats/stats.cc
libstats_la_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier
libstats_la_LDFLAGS = -module -export-dynamic

EXTRA_DIST += mod.stats/stats.h
