## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.9 2003/09/02 15:04:40 dan_karrels Exp $"

lib_LTLIBRARIES += libscanner.la
libscanner_la_SOURCES = \
	mod.scanner/ScannerModule.cc \
	mod.scanner/dbThread.cc \
	mod.scanner/scanner.cc \
	mod.scanner/wingateModule.cc
libscanner_la_LDFLAGS = -module -export-dynamic
libscanner_la_CXXFLAGS = -I$(top_srcdir)/include \
	-I$(top_srcdir)/libgnuworld \
	-I$(top_srcdir)/libnotifier

EXTRA_DIST += \
	mod.scanner/dbThread.h \
	mod.scanner/scanner.h \
	mod.scanner/ScannerModule.h
