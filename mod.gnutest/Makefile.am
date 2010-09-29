## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.7 2003/09/02 00:54:58 jeekay Exp $"

lib_LTLIBRARIES = libgnutest.la
libgnutest_la_SOURCES = gnutest.cc
libgnutest_la_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
libgnutest_la_LDFLAGS = -module -export-dynamic

EXTRA_DIST = gnutest.h
