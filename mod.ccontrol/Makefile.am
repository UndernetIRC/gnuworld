## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.6 2001/02/22 13:08:38 mrbean_ Exp $"

lib_LTLIBRARIES = libccontrol.la
libccontrol_la_SOURCES = ccontrol.cc \
	CControlCommands.cc \
	ACCESSCommand.cc \
	ADDCOMMANDCommand.cc \
	ADDNEWOPERCommand.cc \
	ADDOPERCHANCommand.cc \
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
