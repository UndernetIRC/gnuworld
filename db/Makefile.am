## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.6 2003/06/12 14:56:29 dan_karrels Exp $

lib_LTLIBRARIES = libgnuworldDB.la
libgnuworldDB_la_SOURCES = gnuworldDB.cc \
	pgsqlDB.cc

LIBS = @LIBS@
INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)
