## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.54 2009/06/25 19:05:23 mrbean_ Exp $"

lib_LTLIBRARIES += libcservice.la
libcservice_la_LDFLAGS = -module -export-dynamic
libcservice_la_LIBADD = libgnuworldDB.la
libcservice_la_CXXFLAGS = -I$(top_srcdir)/db -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier

libcservice_la_SOURCES = \
	mod.cservice/banMatcher.cc \
	mod.cservice/cservice.cc \
	mod.cservice/cservice_crypt.cc \
	mod.cservice/networkData.cc \
	mod.cservice/sqlChannel.cc \
	mod.cservice/sqlUser.cc \
	mod.cservice/sqlLevel.cc \
	mod.cservice/sqlBan.cc \
	mod.cservice/sqlPendingChannel.cc \
	mod.cservice/sqlPendingTraffic.cc \
	mod.cservice/csGline.cc \
	mod.cservice/ACCESSCommand.cc \
	mod.cservice/ADDCOMMENTCommand.cc \
	mod.cservice/ADDUSERCommand.cc \
	mod.cservice/BANCommand.cc \
	mod.cservice/BANLISTCommand.cc \
	mod.cservice/CERTCommand.cc \
	mod.cservice/CHANINFOCommand.cc \
	mod.cservice/CLEARMODECommand.cc \
	mod.cservice/DEOPCommand.cc \
	mod.cservice/DEVOICECommand.cc \
	mod.cservice/FORCECommand.cc \
	mod.cservice/HELPCommand.cc \
	mod.cservice/HELLOCommand.cc \
	mod.cservice/INVITECommand.cc \
	mod.cservice/ISREGCommand.cc \
	mod.cservice/JOINCommand.cc \
	mod.cservice/KICKCommand.cc \
	mod.cservice/LBANLISTCommand.cc \
	mod.cservice/LOGINCommand.cc \
	mod.cservice/MODINFOCommand.cc \
	mod.cservice/MOTDCommand.cc \
	mod.cservice/NEWPASSCommand.cc \
	mod.cservice/NOTECommand.cc \
	mod.cservice/OPCommand.cc \
	mod.cservice/OPERJOINCommand.cc \
	mod.cservice/OPERPARTCommand.cc \
	mod.cservice/PARTCommand.cc \
	mod.cservice/PURGECommand.cc \
	mod.cservice/QUOTECommand.cc \
	mod.cservice/RANDOMCommand.cc \
	mod.cservice/REGISTERCommand.cc \
	mod.cservice/REHASHCommand.cc \
	mod.cservice/REMIGNORECommand.cc \
	mod.cservice/REMOVEALLCommand.cc \
	mod.cservice/REMUSERCommand.cc \
	mod.cservice/SAYCommand.cc \
	mod.cservice/SCANHOSTCommand.cc \
	mod.cservice/SCANUNAMECommand.cc \
	mod.cservice/SCANEMAILCommand.cc \
	mod.cservice/SEARCHCommand.cc \
	mod.cservice/SERVNOTICECommand.cc \
	mod.cservice/SETCommand.cc \
	mod.cservice/SHOWCOMMANDSCommand.cc \
	mod.cservice/SHOWIGNORECommand.cc \
	mod.cservice/SHUTDOWNCommand.cc \
	mod.cservice/STATSCommand.cc \
	mod.cservice/STATUSCommand.cc \
	mod.cservice/SUPPORTCommand.cc \
	mod.cservice/SUSPENDCommand.cc \
	mod.cservice/SUSPENDMECommand.cc \
	mod.cservice/TOPICCommand.cc \
	mod.cservice/UNBANCommand.cc \
	mod.cservice/UNFORCECommand.cc \
	mod.cservice/UNSUSPENDCommand.cc \
	mod.cservice/VERIFYCommand.cc \
	mod.cservice/VOICECommand.cc \
	mod.cservice/WHITELISTCommand.cc \
	mod.cservice/MODECommand.cc

EXTRA_DIST += \
	mod.cservice/banMatcher.h \
	mod.cservice/constants.h \
	mod.cservice/cserviceCommands.h \
	mod.cservice/cservice_config.h \
	mod.cservice/cservice_confvars.h \
	mod.cservice/cservice_crypt.h \
	mod.cservice/cservice.h \
	mod.cservice/levels.h \
	mod.cservice/networkData.h \
	mod.cservice/responses.h \
	mod.cservice/sqlBan.h \
	mod.cservice/sqlChannel.h \
	mod.cservice/sqlLevel.h \
	mod.cservice/sqlPendingChannel.h \
	mod.cservice/sqlPendingTraffic.h \
	mod.cservice/sqlUser.h \
	mod.cservice/csGline.h \
	mod.cservice/README \
	mod.cservice/RELEASE.NOTES
