## Process this file with automake to produce Makefile.in
## "$Id: Makefile.am,v 1.3 2002/05/31 15:07:19 dan_karrels Exp $"

lib_LTLIBRARIES = libgnuworld_db.la
libgnuworld_db_la_SOURCES = gnuworldDB.cc pgsqlDB.cc

libgnuworld_db_la_LDFLAGS = -version-info 0:0:0 -rpath $(bindir) -static\
	-L@PGSQL_LIB@ -lpq++
INCLUDES = -I@PGSQL_INCLUDE@
