## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.4 2001/02/20 22:26:58 mrbean_ Exp $"

lib_LTLIBRARIES = libccontrol.la
libccontrol_la_SOURCES = ccontrol.cc \
	CControlCommands.cc
libccontrol_la_LDFLAGS = -module -export-dynamic -L/usr/local/pgsql/lib 	-lpq++
INCLUDES = -I/usr/local/pgsql/include