## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.25 2001/01/27 20:16:40 dan_karrels Exp $"

lib_LTLIBRARIES = libcservice.la
libcservice_la_LDFLAGS = -module -export-dynamic -L/usr/local/pgsql/lib \
	-lpq++

libcservice_la_SOURCES = cservice.cc \
	networkData.cc \
	sqlChannel.cc \
	sqlUser.cc \
	sqlLevel.cc \
	sqlBan.cc \
	ACCESSCommand.cc \
	ADDUSERCommand.cc \
	BANCommand.cc \
	BANLISTCommand.cc \
	CHANINFOCommand.cc \
	DEOPCommand.cc \
	DEVOICECommand.cc \
	FORCECommand.cc \
	INVITECommand.cc \
	ISREGCommand.cc \
	KICKCommand.cc \
	LBANLISTCommand.cc \
	LOGINCommand.cc \
	MODINFOCommand.cc \
	MOTDCommand.cc \
	NEWPASSCommand.cc \
	OPCommand.cc \
	RANDOMCommand.cc \
	REGISTERCommand.cc \
	PURGECommand.cc \
	REMUSERCommand.cc \
	SAYCommand.cc \
	SEARCHCommand.cc \
	SERVNOTICECommand.cc \
	SETCommand.cc \
	SHOWCOMMANDSCommand.cc \
	SHOWIGNORECommand.cc \
	STATUSCommand.cc \
	SUSPENDCommand.cc \
	TOPICCommand.cc \
	UNBANCommand.cc \
	UNFORCECommand.cc \
	UNSUSPENDCommand.cc \
	VERIFYCommand.cc \
	VOICECommand.cc

INCLUDES = -I/usr/local/pgsql/include
