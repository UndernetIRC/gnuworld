## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.8 2003/07/10 00:58:24 dan_karrels Exp $"

lib_LTLIBRARIES = libscanner.la
libscanner_la_SOURCES = ScannerModule.cc \
	dbThread.cc \
	scanner.cc \
	wingateModule.cc
libscanner_la_LDFLAGS = -module -export-dynamic

EXTRA_DIST = dbThread.h \
	scanner.h \
	ScannerModule.h
