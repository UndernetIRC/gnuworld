## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.11 2005/03/18 16:19:57 mrbean_ Exp $

lib_LTLIBRARIES = libgnuworldDB.la
libgnuworldDB_la_SOURCES = gnuworldDB.cc \
	pgsqlDB.cc
libgnuworldDB_la_CXXFLAGS = -I$(PGSQL_INCLUDE)  -DHAVE_NAMESPACE_STD -DHAVE_CXX_STRING_HEADER -DDLLIMPORT=""

EXTRA_DIST = gnuworldDB.h \
	pgsqlDB.h
