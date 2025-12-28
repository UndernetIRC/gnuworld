
lib_LTLIBRARIES += libdronescan.la
libdronescan_la_LDFLAGS = -module -export-dynamic
libdronescan_la_LIBADD = libgnuworldDB.la
libdronescan_la_CXXFLAGS = -I$(top_srcdir)/db \
	-I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier \
	-I${top_srcdir}/include

libdronescan_la_SOURCES = 	\
	mod.dronescan/dronescan.cc		\
	mod.dronescan/Convert.cc		\
	mod.dronescan/sqlFakeClient.cc	\
	mod.dronescan/sqlUser.cc		\
	mod.dronescan/ACCESSCommand.cc	\
	mod.dronescan/ADDEXCEPTIONALCHANNELCommand.cc\
	mod.dronescan/REMEXCEPTIONALCHANNELCommand.cc\
	mod.dronescan/RELOADCommand.cc\
	mod.dronescan/ADDUSERCommand.cc	\
	mod.dronescan/ANALYSECommand.cc	\
	mod.dronescan/CHECKCommand.cc		\
	mod.dronescan/FAKECommand.cc		\
	mod.dronescan/LISTCommand.cc		\
	mod.dronescan/MODUSERCommand.cc	\
	mod.dronescan/QUOTECommand.cc		\
	mod.dronescan/REMUSERCommand.cc	\
	mod.dronescan/STATUSCommand.cc	\
	mod.dronescan/ABNORMALSTest.cc	\
	mod.dronescan/COMMONREALTest.cc	\
	mod.dronescan/HASALLOPTest.cc		\
	mod.dronescan/HASOPTest.cc		\
	mod.dronescan/MAXCHANSTest.cc		\
	mod.dronescan/RANGETest.cc		\
	mod.dronescan/jfChannel.cc

EXTRA_DIST += \
	mod.dronescan/activeChannel.h \
	mod.dronescan/clientData.h \
	mod.dronescan/constants.h \
	mod.dronescan/Convert.h \
	mod.dronescan/dronescanCommands.h \
	mod.dronescan/dronescan.h \
	mod.dronescan/dronescanTests.h \
	mod.dronescan/levels.h \
	mod.dronescan/sqlFakeClient.h \
	mod.dronescan/sqlUser.h \
	mod.dronescan/ChangeLog \
	mod.dronescan/README
