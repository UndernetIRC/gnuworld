## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.23 2001/07/29 13:33:20 mrbean_ Exp $"

lib_LTLIBRARIES = libccontrol.la
libccontrol_la_SOURCES = ccontrol.cc \
	CControlCommands.cc \
	ACCESSCommand.cc \
	ADDCOMMANDCommand.cc \
	ADDOPERCHANCommand.cc \
	ADDSERVERCommand.cc \
	ADDUSERCommand.cc \
	AuthInfo.cc \
	ccException.cc \
	ccLogin.cc \
	ccGline.cc \
	ccServer.cc \
	ccUser.cc \
	CHANINFOCommand.cc \
	CHECKNETCommand.cc \
	CLEARCHANCommand.cc \
	COMMANDSCommand.cc \
	DEAUTHCommand.cc \
	DEOPCommand.cc \
	EXCEPTIONCommand.cc \
	FORCEGLINECommand.cc \
	GLINECommand.cc \
	HELPCommand.cc \
	INVITECommand.cc \
	JUPECommand.cc \
	KICKCommand.cc \
	LASTCOMCommand.cc \
	LEARNNETCommand.cc \
	LISTCommand.cc \
	LISTHOSTSCommand.cc \
	LISTIGNORESCommand.cc \
	LISTOPERCHANSCommand.cc \
	LOGINCommand.cc \
	MODECommand.cc \
	MODERATECommand.cc \
	MODUSERCommand.cc \
	NEWPASSCommand.cc \
	OPCommand.cc \
	REMGLINECommand.cc \
	REMOPERCHANCommand.cc \
	REMCOMMANDCommand.cc \
	REMOVEIGNORECommand.cc \
	REMSERVERCommand.cc \
	REMUSERCommand.cc \
	SCANGLINECommand.cc \
	SUSPENDCommand.cc \
	TRANSLATECommand.cc \
	UNMODERATECommand.cc \
	UNSUSPENDCommand.cc \
	WHOISCommand.cc
    	
libccontrol_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++
INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)
