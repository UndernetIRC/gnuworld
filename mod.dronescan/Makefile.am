lib_LTLIBRARIES = libdronescan.la
libdronescan_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++
libdronescan_la_CXXFLAGS = -I@PGSQL_INCLUDE@

libdronescan_la_SOURCES = 	\
	dronescan.cc		\
	sqlUser.cc		\
	ABNORMALSTest.cc	\
	ACCESSCommand.cc	\
	ADDUSERCommand.cc	\
	CHECKCommand.cc		\
	COMMONREALTest.cc	\
	HASALLOPTest.cc		\
	HASOPTest.cc		\
	LISTCommand.cc		\
	MAXCHANSTest.cc		\
	MODUSERCommand.cc	\
	QUOTECommand.cc		\
	RANGETest.cc		\
	REMUSERCommand.cc	\
	STATUSCommand.cc

EXTRA_DIST = clientData.h \
	dronescanCommands.h \
	dronescan.h \
	dronescanTests.h \
	levels.h \
	Timer.h \
	ChangeLog
