## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.6 2002/09/30 16:55:57 dan_karrels Exp $"

lib_LTLIBRARIES = libscanner.la
libscanner_la_SOURCES = ScannerModule.cc \
	dbThread.cc \
	scanner.cc \
	wingateModule.cc

libscanner_la_LDFLAGS = -module -export-dynamic

INCLUDES = $(INCLTDL)
