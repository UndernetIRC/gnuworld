lib_LTLIBRARIES = libdronescan.la
libdronescan_la_LDFLAGS = -module -export-dynamic -L$(PGSQL_LIB) -lpq++
libdronescan_la_CXXFLAGS = -I$(PGSQL_INCLUDE) -I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/include

libdronescan_la_SOURCES = 	\
	dronescan.cc		\
	Convert.cc		\
	sqlFakeClient.cc	\
	sqlUser.cc		\
	ACCESSCommand.cc	\
	ADDUSERCommand.cc	\
	ANALYSECommand.cc	\
	CHECKCommand.cc		\
	FAKECommand.cc		\
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

EXTRA_DIST = activeChannel.h \
	clientData.h \
	constants.h \
	Convert.h \
	dronescanCommands.h \
	dronescan.h \
	dronescanTests.h \
	levels.h \
	sqlFakeClient.h \
	sqlUser.h \
	Timer.h \
	ChangeLog \
	Updates
