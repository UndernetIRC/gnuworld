## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.10 2003/11/27 18:29:30 dan_karrels Exp $

lib_LTLIBRARIES = libgnuworldDB.la
libgnuworldDB_la_SOURCES = gnuworldDB.cc \
	pgsqlDB.cc
libgnuworldDB_la_CXXFLAGS = -I$(PGSQL_INCLUDE)

EXTRA_DIST = gnuworldDB.h \
	pgsqlDB.h
