lib_LTLIBRARIES = libdronescan.la
libdronescan_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++
libdronescan_la_CXXFLAGS = -I@PGSQL_INCLUDE@ -I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/include

libdronescan_la_SOURCES = 	\
	dronescan.cc		\
	sqlUser.cc		\
	ACCESSCommand.cc	\
	ADDUSERCommand.cc	\
	CHECKCommand.cc		\
	LISTCommand.cc		\
	MODUSERCommand.cc	\
	QUOTECommand.cc		\
	REMUSERCommand.cc	\
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
