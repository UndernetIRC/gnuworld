## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.36 2002/05/25 15:03:57 mrbean_ Exp $"

lib_LTLIBRARIES = libccontrol.la
libccontrol_la_SOURCES = ccontrol.cc \
	CControlCommands.cc \
	ADDCOMMANDCommand.cc \
	ADDOPERCHANCommand.cc \
	ADDSERVERCommand.cc \
	ADDUSERCommand.cc \
	ccBadChannel.cc \
	ccException.cc \
	ccFloodData.cc \
	ccGate.cc \
	ccGline.cc \
	ccLog.cc \
	ccServer.cc \
	ccUser.cc \
	CHANINFOCommand.cc \
	CHECKNETCommand.cc \
	CLEARCHANCommand.cc \
	COMMANDSCommand.cc \
	CONFIGCommand.cc \
	DEAUTHCommand.cc \
	DEOPCommand.cc \
	EXCEPTIONCommand.cc \
	FORCEGLINECommand.cc \
	GCHANCommand.cc \
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
	MAXUSERSCommand.cc \
	MODECommand.cc \
	MODERATECommand.cc \
	MODUSERCommand.cc \
	NEWPASSCommand.cc \
	NOMODECommand.cc \
	OPCommand.cc \
	REMGCHANCommand.cc \
	REMGLINECommand.cc \
	REMOPERCHANCommand.cc \
	REMCOMMANDCommand.cc \
	REMOVEIGNORECommand.cc \
	REMSERVERCommand.cc \
	REMUSERCommand.cc \
	SCANCommand.cc \
	SCANGLINECommand.cc \
	SHUTDOWNCommand.cc \
	STATUSCommand.cc \
	SUSPENDCommand.cc \
	TRANSLATECommand.cc \
	UNMODERATECommand.cc \
	UNSUSPENDCommand.cc \
	USERINFOCommand.cc \
	WHOISCommand.cc


libccontrol_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++ 
INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)
CFLAGS = @CFLAGS@ -D_THREAD_SAFE
CXXFLAGS = @CXXFLAGS@ -D_THREAD_SAFE
