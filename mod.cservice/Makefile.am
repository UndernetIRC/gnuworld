## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.49 2003/09/02 00:54:58 jeekay Exp $"

lib_LTLIBRARIES = libcservice.la
libcservice_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ \
	-lpq++
libcservice_la_CXXFLAGS = -I@PGSQL_INCLUDE@ -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld

libcservice_la_SOURCES = cservice.cc \
	networkData.cc \
	sqlChannel.cc \
	sqlUser.cc \
	sqlLevel.cc \
	sqlBan.cc \
	sqlPendingChannel.cc \
	sqlPendingTraffic.cc \
	ACCESSCommand.cc \
	ADDCOMMENTCommand.cc \
	ADDUSERCommand.cc \
	BANCommand.cc \
	BANLISTCommand.cc \
	CHANINFOCommand.cc \
	CLEARMODECommand.cc \
	DEOPCommand.cc \
	DEVOICECommand.cc \
	FORCECommand.cc \
	HELPCommand.cc \
	INVITECommand.cc \
	ISREGCommand.cc \
	JOINCommand.cc \
	KICKCommand.cc \
	LBANLISTCommand.cc \
	LOGINCommand.cc \
	MODINFOCommand.cc \
	MOTDCommand.cc \
	NEWPASSCommand.cc \
	NOTECommand.cc \
	OPCommand.cc \
	OPERJOINCommand.cc \
	OPERPARTCommand.cc \
	PARTCommand.cc \
	PURGECommand.cc \
	QUOTECommand.cc \
	RANDOMCommand.cc \
	REGISTERCommand.cc \
	REHASHCommand.cc \
	REMIGNORECommand.cc \
	REMOVEALLCommand.cc \
	REMUSERCommand.cc \
	SAYCommand.cc \
	SCANHOSTCommand.cc \
	SCANUNAMECommand.cc \
	SEARCHCommand.cc \
	SERVNOTICECommand.cc \
	SETCommand.cc \
	SHOWCOMMANDSCommand.cc \
	SHOWIGNORECommand.cc \
	SHUTDOWNCommand.cc \
	STATSCommand.cc \
	STATUSCommand.cc \
	SUPPORTCommand.cc \
	SUSPENDCommand.cc \
	SUSPENDMECommand.cc \
	TOPICCommand.cc \
	UNBANCommand.cc \
	UNFORCECommand.cc \
	UNSUSPENDCommand.cc \
	VERIFYCommand.cc \
	VOICECommand.cc

EXTRA_DIST = constants.h \
	cserviceCommands.h \
	cservice_config.h \
	cservice.h \
	levels.h \
	networkData.h \
	responses.h \
	sqlBan.h \
	sqlChannel.h \
	sqlLevel.h \
	sqlPendingChannel.h \
	sqlPendingTraffic.h \
	sqlUser.h \
	README \
	RELEASE.NOTES
