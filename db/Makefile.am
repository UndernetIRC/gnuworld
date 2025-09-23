## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.14 2007/09/18 15:34:47 dan_karrels Exp $

lib_LTLIBRARIES += libgnuworldDB.la
libgnuworldDB_la_SOURCES = \
	db/gnuworldDB.cc \
	db/pgsqlDB.cc
libgnuworldDB_la_CXXFLAGS = \
	-I$(top_srcdir)/libgnuworld \
	-I$(top_srcdir)/include
libgnuworldDB_la_LDFLAGS = -L$(pgsql_libdir) -lpq

EXTRA_DIST += \
	db/gnuworldDB.h \
	db/pgsqlDB.h \
	db/dbHandle.h