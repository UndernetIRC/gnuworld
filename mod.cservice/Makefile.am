## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.18 2001/01/08 04:13:04 gte Exp $"

lib_LTLIBRARIES = libcservice.la
libcservice_la_LDFLAGS = -module -export-dynamic -L/usr/local/pgsql/lib \
	-lpq++

libcservice_la_SOURCES = cservice.cc \
	md5hash.cc \
	networkData.cc \
	LOGINCommand.cc \
	sqlChannel.cc \
	sqlUser.cc \
	sqlLevel.cc \
	ACCESSCommand.cc \
	CHANINFOCommand.cc \
	ISREGCommand.cc \
	MOTDCommand.cc \
	RANDOMCommand.cc \
	SEARCHCommand.cc \
	SHOWCOMMANDSCommand.cc \
	SHOWIGNORECommand.cc \
	VERIFYCommand.cc \
	OPCommand.cc \
	DEOPCommand.cc \
	VOICECommand.cc \
	DEVOICECommand.cc \
	ADDUSERCommand.cc \
	REMUSERCommand.cc \
	MODINFOCommand.cc \
	SETCommand.cc \
	TOPICCommand.cc \
	INVITECommand.cc \
	BANLISTCommand.cc \
	KICKCommand.cc \
	REGISTERCommand.cc \
	FORCECommand.cc \
	UNFORCECommand.cc \
	SERVNOTICECommand.cc \
	STATUSCommand.cc
INCLUDES = -I/usr/local/pgsql/include
