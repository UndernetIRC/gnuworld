## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.14 2007/09/18 15:34:47 dan_karrels Exp $

lib_LTLIBRARIES = libgnuworldDB.la
libgnuworldDB_la_SOURCES = gnuworldDB.cc \
	pgsqlDB.cc
##libgnuworldDB_la_CXXFLAGS = -I$(pgsql_includedir)
libgnuworldDB_la_LDFLAGS = -L$(pgsql_libdir) -lpq

EXTRA_DIST = gnuworldDB.h \
	pgsqlDB.h \
	dbHandle.h
