## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.6 2000/12/22 22:22:49 gte Exp $"

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
	VOICECommand.cc
INCLUDES = -I/usr/local/pgsql/include
