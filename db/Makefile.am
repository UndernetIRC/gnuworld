## Process this file with automake to produce Makefile.in
## "$Id: Makefile.am,v 1.4 2002/06/03 22:19:31 dan_karrels Exp $"

lib_LTLIBRARIES = libgnuworld_db.la
libgnuworld_db_la_SOURCES = gnuworldDB.cc pgsqlDB.cc

libgnuworld_db_la_LDFLAGS = -version-info 0:0:0 -rpath $(bindir) \
	-L@PGSQL_LIB@ -lpq++
INCLUDES = -I@PGSQL_INCLUDE@
