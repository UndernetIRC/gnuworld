## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.1 2002/08/10 13:55:19 jeekay Exp $"

lib_LTLIBRARIES = libnickserv.la
libnickserv_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++

libnickserv_la_SOURCES = nickserv.cc \
	sqlManager.cc \
	sqlUser.cc

INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)
