## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.7 2001/02/24 19:22:08 mrbean_ Exp $"

lib_LTLIBRARIES = libccontrol.la
libccontrol_la_SOURCES = ccontrol.cc \
	CControlCommands.cc \
	ACCESSCommand.cc \
	ADDCOMMANDCommand.cc \
	ADDNEWOPERCommand.cc \
	ADDOPERCHANCommand.cc \
	ccUser.cc \
	CHANINFOCommand.cc \
	DEAUTHCommand.cc \
	GLINECommand.cc \
	HELPCommand.cc \
	INVITECommand.cc \
	JUPECommand.cc \
	KICKCommand.cc \
	LISTOPERCHANSCommand.cc \
	LOGINCommand.cc \
	MODECommand.cc \
	MODOPERCommand.cc \
	NEWPASSCommand.cc \
	REMGLINECommand.cc \
	REMOPERCHANCommand.cc \
	REMOVECOMMANDCommand.cc \
	REMOVEOPERCommand.cc \
	SCANGLINECommand.cc \
	SUSPENDOPERCommand.cc \
	TRANSLATECommand.cc \
	UNSUSPENDOPERCommand.cc \
	WHOISCommand.cc
    	
libccontrol_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++
INCLUDES = -I@PGSQL_INCLUDE@
