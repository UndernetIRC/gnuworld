## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.5 2001/02/21 00:14:43 dan_karrels Exp $"

lib_LTLIBRARIES = libccontrol.la
libccontrol_la_SOURCES = ccontrol.cc \
	CControlCommands.cc
libccontrol_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++
INCLUDES = -I@PGSQL_INCLUDE@
