## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.9 2003/09/02 15:04:40 dan_karrels Exp $"

lib_LTLIBRARIES = libscanner.la
libscanner_la_SOURCES = ScannerModule.cc \
	dbThread.cc \
	scanner.cc \
	wingateModule.cc
libscanner_la_LDFLAGS = -module -export-dynamic
libscanner_la_CXXFLAGS = -I$(top_srcdir)/include \
	-I$(top_srcdir)/libgnuworld

EXTRA_DIST = dbThread.h \
	scanner.h \
	ScannerModule.h
