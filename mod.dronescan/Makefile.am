lib_LTLIBRARIES = libdronescan.la
libdronescan_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++
libdronescan_la_CXXFLAGS = -I@PGSQL_INCLUDE@

libdronescan_la_SOURCES = 	\
	dronescan.cc		\
	sqlUser.cc		\
	ACCESSCommand.cc	\
	CHECKCommand.cc		\
	LISTCommand.cc		\
	STATUSCommand.cc	\
	ABNORMALSTest.cc	\
	COMMONREALTest.cc	\
	HASALLOPTest.cc		\
	HASOPTest.cc		\
	MAXCHANSTest.cc		\
	RANGETest.cc

EXTRA_DIST = clientData.h \
	dronescanCommands.h \
	dronescan.h \
	dronescanTests.h \
	levels.h \
	Timer.h \
	ChangeLog
