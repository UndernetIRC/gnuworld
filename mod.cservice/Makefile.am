## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.38 2001/08/13 00:26:57 gte Exp $"

lib_LTLIBRARIES = libcservice.la
libcservice_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ \
	-lpq++

libcservice_la_SOURCES = cservice.cc \
	networkData.cc \
	sqlChannel.cc \
	sqlUser.cc \
	sqlLevel.cc \
	sqlBan.cc \
	sqlPendingChannel.cc \
	sqlPendingTraffic.cc \
	ACCESSCommand.cc \
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
	SEARCHCommand.cc \
	SERVNOTICECommand.cc \
	SETCommand.cc \
	SHOWCOMMANDSCommand.cc \
	SHOWIGNORECommand.cc \
	STATUSCommand.cc \
	SUPPORTCommand.cc \
	SUSPENDCommand.cc \
	TOPICCommand.cc \
	UNBANCommand.cc \
	UNFORCECommand.cc \
	UNSUSPENDCommand.cc \
	VERIFYCommand.cc \
	VOICECommand.cc

INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)

