## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.59 2009/06/09 05:55:55 hidden1 Exp $"

lib_LTLIBRARIES += libccontrol.la
libccontrol_la_SOURCES = \
	mod.ccontrol/ccontrol.cc \
	mod.ccontrol/CControlCommands.cc \
	mod.ccontrol/ADDCOMMANDCommand.cc \
	mod.ccontrol/ADDOPERCHANCommand.cc \
	mod.ccontrol/ADDSERVERCommand.cc \
	mod.ccontrol/ADDUSERCommand.cc \
	mod.ccontrol/ccBadChannel.cc \
	mod.ccontrol/ccException.cc \
	mod.ccontrol/ccFloodData.cc \
	mod.ccontrol/ccGline.cc \
	mod.ccontrol/ccLog.cc \
	mod.ccontrol/ccServer.cc \
	mod.ccontrol/ccUser.cc \
	mod.ccontrol/ccontrol_generic.cc \
	mod.ccontrol/CHANINFOCommand.cc \
	mod.ccontrol/CHECKNETCommand.cc \
	mod.ccontrol/CLEARCHANCommand.cc \
	mod.ccontrol/COMMANDSCommand.cc \
	mod.ccontrol/CONFIGCommand.cc \
	mod.ccontrol/DEAUTHCommand.cc \
	mod.ccontrol/DEOPCommand.cc \
	mod.ccontrol/FORCECHANGLINECommand.cc \
	mod.ccontrol/FORCEGLINECommand.cc \
	mod.ccontrol/GCHANCommand.cc \
	mod.ccontrol/GLINECommand.cc \
	mod.ccontrol/HELPCommand.cc \
	mod.ccontrol/INVITECommand.cc \
	mod.ccontrol/JUPECommand.cc \
	mod.ccontrol/KICKCommand.cc \
	mod.ccontrol/LASTCOMCommand.cc \
	mod.ccontrol/LEARNNETCommand.cc \
	mod.ccontrol/LISTCommand.cc \
	mod.ccontrol/LISTHOSTSCommand.cc \
	mod.ccontrol/LISTIGNORESCommand.cc \
	mod.ccontrol/LISTOPERCHANSCommand.cc \
	mod.ccontrol/LISTUSERSCommand.cc \
	mod.ccontrol/LOGINCommand.cc \
	mod.ccontrol/MAXUSERSCommand.cc \
	mod.ccontrol/MODECommand.cc \
	mod.ccontrol/MODERATECommand.cc \
	mod.ccontrol/MODUSERCommand.cc \
	mod.ccontrol/NEWPASSCommand.cc \
	mod.ccontrol/NOMODECommand.cc \
	mod.ccontrol/OPCommand.cc \
	mod.ccontrol/REMGCHANCommand.cc \
	mod.ccontrol/REMGLINECommand.cc \
	mod.ccontrol/REMOPERCHANCommand.cc \
	mod.ccontrol/REMCOMMANDCommand.cc \
	mod.ccontrol/REMOVEIGNORECommand.cc \
	mod.ccontrol/REMSERVERCommand.cc \
	mod.ccontrol/REMSGLINECommand.cc \
	mod.ccontrol/REMUSERCommand.cc \
	mod.ccontrol/REOPCommand.cc \
	mod.ccontrol/SAYCommand.cc \
	mod.ccontrol/SCANCommand.cc \
	mod.ccontrol/SCANGLINECommand.cc \
	mod.ccontrol/SCHANGLINECommand.cc \
	mod.ccontrol/SGLINECommand.cc \
	mod.ccontrol/SHUTDOWNCommand.cc \
	mod.ccontrol/STATUSCommand.cc \
	mod.ccontrol/SUSPENDCommand.cc \
	mod.ccontrol/TRANSLATECommand.cc \
	mod.ccontrol/TOPICCommand.cc \
	mod.ccontrol/UNJUPECommand.cc \
	mod.ccontrol/UNMODERATECommand.cc \
	mod.ccontrol/UNSUSPENDCommand.cc \
	mod.ccontrol/USERINFOCommand.cc \
	mod.ccontrol/WHOISCommand.cc \
	mod.ccontrol/ANNOUNCECommand.cc \
	mod.ccontrol/LIMITSCommand.cc

libccontrol_la_CXXFLAGS = -I$(top_srcdir)/db -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier
libccontrol_la_LDFLAGS = -module -export-dynamic
libccontrol_la_LIBADD = libgnuworldDB.la

EXTRA_DIST += \
	mod.ccontrol/ccBadChannel.h \
	mod.ccontrol/ccException.h \
	mod.ccontrol/ccFloodData.h \
	mod.ccontrol/ccGline.h \
	mod.ccontrol/ccLog.h \
	mod.ccontrol/CControlCommands.h \
	mod.ccontrol/ccontrol.h \
	mod.ccontrol/ccServer.h \
	mod.ccontrol/ccUserData.h \
	mod.ccontrol/ccUser.h \
	mod.ccontrol/ccontrol_generic.h \
	mod.ccontrol/CommandsDec.h \
	mod.ccontrol/commLevels.h \
	mod.ccontrol/Constants.h \
	mod.ccontrol/README
