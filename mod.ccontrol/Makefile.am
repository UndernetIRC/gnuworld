## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.59 2009/06/09 05:55:55 hidden1 Exp $"

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
	ccontrol_generic.cc \
	CHANINFOCommand.cc \
	CHECKNETCommand.cc \
	CLEARCHANCommand.cc \
	COMMANDSCommand.cc \
	CONFIGCommand.cc \
	DEAUTHCommand.cc \
	DEOPCommand.cc \
	EXCEPTIONCommand.cc \
	FORCECHANGLINECommand.cc \
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
	LISTUSERSCommand.cc \
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
	SCHANGLINECommand.cc \
	SGLINECommand.cc \
	SHUTDOWNCommand.cc \
	STATUSCommand.cc \
	SUSPENDCommand.cc \
	TRANSLATECommand.cc \
	UNJUPECommand.cc \
	UNMODERATECommand.cc \
	UNSUSPENDCommand.cc \
	USERINFOCommand.cc \
	WHOISCommand.cc \
	SHELLSCommand.cc \
	ANNOUNCECommand.cc \
	IP6Command.cc

libccontrol_la_CXXFLAGS = -I$(top_srcdir)/db -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld
libccontrol_la_LDFLAGS = -module -export-dynamic -L$(top_builddir)/db \
	-lgnuworldDB 

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
	ccontrol_generic.h \
	CommandsDec.h \
	commLevels.h \
	Constants.h \
	README
