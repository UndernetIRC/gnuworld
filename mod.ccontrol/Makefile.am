## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.8 2001/03/10 18:33:23 mrbean_ Exp $"

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
	DEOPCommand.cc \
	GLINECommand.cc \
	HELPCommand.cc \
	INVITECommand.cc \
	JUPECommand.cc \
	KICKCommand.cc \
	LISTHOSTSCommand.cc \
	LISTOPERCHANSCommand.cc \
	LOGINCommand.cc \
	MODECommand.cc \
	MODERATECommand.cc \
	MODOPERCommand.cc \
	NEWPASSCommand.cc \
	OPCommand.cc \
	REMGLINECommand.cc \
	REMOPERCHANCommand.cc \
	REMOVECOMMANDCommand.cc \
	REMOVEOPERCommand.cc \
	SCANGLINECommand.cc \
	SUSPENDOPERCommand.cc \
	TRANSLATECommand.cc \
	UNMODERATECommand.cc \
	UNSUSPENDOPERCommand.cc \
	WHOISCommand.cc
    	
libccontrol_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++
INCLUDES = -I@PGSQL_INCLUDE@
