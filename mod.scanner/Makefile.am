## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.7 2003/06/28 01:21:21 dan_karrels Exp $"

lib_LTLIBRARIES = libscanner.la
libscanner_la_SOURCES = ScannerModule.cc \
	dbThread.cc \
	scanner.cc \
	wingateModule.cc

libscanner_la_LDFLAGS = -module -export-dynamic

INCLUDES = $(INCLTDL)

EXTRA_DIST = dbThread.h \
	scanner.h \
	ScannerModule.h
