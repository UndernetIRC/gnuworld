## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.20 2001/01/12 22:16:21 dan_karrels Exp $"

lib_LTLIBRARIES = libcservice.la
libcservice_la_LDFLAGS = -module -export-dynamic -L/usr/local/pgsql/lib \
	-lpq++

libcservice_la_SOURCES = cservice.cc \
	networkData.cc \
	sqlChannel.cc \
	sqlUser.cc \
	sqlLevel.cc \
	ACCESSCommand.cc \
	ADDUSERCommand.cc \
	BANLISTCommand.cc \
	CHANINFOCommand.cc \
	DEOPCommand.cc \
	DEVOICECommand.cc \
	FORCECommand.cc \
	INVITECommand.cc \
	ISREGCommand.cc \
	KICKCommand.cc \
	LOGINCommand.cc \
	MODINFOCommand.cc \
	MOTDCommand.cc \
	OPCommand.cc \
	RANDOMCommand.cc \
	REGISTERCommand.cc \
	REMUSERCommand.cc \
	SEARCHCommand.cc \
	SERVNOTICECommand.cc \
	SETCommand.cc \
	SHOWCOMMANDSCommand.cc \
	SHOWIGNORECommand.cc \
	STATUSCommand.cc \
	SUSPENDCommand.cc \
	TOPICCommand.cc \
	UNFORCECommand.cc \
	UNSUSPENDCommand.cc \
	VERIFYCommand.cc \
	VOICECommand.cc
INCLUDES = -I/usr/local/pgsql/include
