## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.50 2003/11/27 18:29:30 dan_karrels Exp $"

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
	REMSGLINECommand.cc \
	REMUSERCommand.cc \
	REOPCommand.cc \
	SAYCommand.cc \
	SCANCommand.cc \
	SCANGLINECommand.cc \
	SGLINECommand.cc \
	SHUTDOWNCommand.cc \
	STATUSCommand.cc \
	SUSPENDCommand.cc \
	TRANSLATECommand.cc \
	UNMODERATECommand.cc \
	UNSUSPENDCommand.cc \
	USERINFOCommand.cc \
	WHOISCommand.cc

libccontrol_la_CXXFLAGS = -I$(PGSQL_INCLUDE) -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld
libccontrol_la_LDFLAGS = -module -export-dynamic -L$(PGSQL_LIB) -lpq++ 

EXTRA_DIST = ccBadChannel.h \
	ccException.h \
	ccFloodData.h \
	ccGline.h \
	ccLog.h \
	CControlCommands.h \
	ccontrol.h \
	ccServer.h \
	ccUserData.h \
	ccUser.h \
	CommandsDec.h \
	commLevels.h \
	Constants.h \
	README
