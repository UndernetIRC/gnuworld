## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.11 2001/04/30 23:44:42 mrbean_ Exp $"

lib_LTLIBRARIES = libccontrol.la
libccontrol_la_SOURCES = ccontrol.cc \
	CControlCommands.cc \
	ACCESSCommand.cc \
	ADDCOMMANDCommand.cc \
	ADDNEWOPERCommand.cc \
	ADDNEWSERVERCommand.cc \
	ADDOPERCHANCommand.cc \
	ccGline.cc \
	ccServer.cc \
	ccUser.cc \
	CHANINFOCommand.cc \
	CLEARCHANCommand.cc \
	DEAUTHCommand.cc \
	DEOPCommand.cc \
	GLINECommand.cc \
	HELPCommand.cc \
	INVITECommand.cc \
	JUPECommand.cc \
	KICKCommand.cc \
	LEARNNETWORKCommand.cc \
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
