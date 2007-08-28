## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.12 2007/08/28 16:09:58 dan_karrels Exp $

lib_LTLIBRARIES = libgnuworldDB.la
libgnuworldDB_la_SOURCES = gnuworldDB.cc \
	pgsqlDB.cc
libgnuworldDB_la_CXXFLAGS = -I$(pgsql_include)
libgnuworldDB_la_LDFLAGS = -L$(pgsql_lib) -lpq

EXTRA_DIST = gnuworldDB.h \
	pgsqlDB.h \
	dbHandle.h
