## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.5 2002/08/08 21:44:00 dan_karrels Exp $"

lib_LTLIBRARIES = libscanner.la
libscanner_la_SOURCES = ScannerModule.cc \
	dbThread.cc \
	scanner.cc \
	wingateModule.cc

libscanner_la_LDFLAGS = -module -export-dynamic $(threadLib)

INCLUDES = $(INCLTDL)
