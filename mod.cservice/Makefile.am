## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.12 2000/12/31 05:06:27 gte Exp $"

lib_LTLIBRARIES = libcservice.la
libcservice_la_LDFLAGS = -module -export-dynamic -L/usr/local/pgsql/lib \
	-lpq++

libcservice_la_SOURCES = cservice.cc \
	sqlChannel.cc \
	sqlUser.cc \
	sqlLevel.cc \
	ACCESSCommand.cc \
	CHANINFOCommand.cc \
	ISREGCommand.cc \
	LOGINCommand.cc \
	MOTDCommand.cc \
	RANDOMCommand.cc \
	SEARCHCommand.cc \
	SHOWCOMMANDSCommand.cc \
	SHOWIGNORECommand.cc \
	VERIFYCommand.cc \
	OPCommand.cc \
	VOICECommand.cc \
	ADDUSERCommand.cc \
	REMUSERCommand.cc \
	MODINFOCommand.cc \
	SETCommand.cc \
	TOPICCommand.cc \
	INVITECommand.cc \
	BANLISTCommand.cc \
	KICKCommand.cc 
INCLUDES = -I/usr/local/pgsql/include
