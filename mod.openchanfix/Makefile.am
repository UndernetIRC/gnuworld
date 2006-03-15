## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.1 2006/03/15 02:50:37 buzlip01 Exp $"

lib_LTLIBRARIES = libchanfix.la

libchanfix_la_SOURCES = chanfix.cc \
	ADDFLAGCommand.cc \
	ADDHOSTCommand.cc \
	ADDNOTECommand.cc \
	ADDUSERCommand.cc \
	ALERTCommand.cc \
	BLOCKCommand.cc \
	CHANFIXCommand.cc \
	CHECKCommand.cc \
	DEBUGCommand.cc \
	DELFLAGCommand.cc \
	DELHOSTCommand.cc \
	DELNOTECommand.cc \
	DELUSERCommand.cc \
	HELPCommand.cc \
	HISTORYCommand.cc \
	INFOCommand.cc \
	INVITECommand.cc \
	LISTHOSTSCommand.cc \
	OPLISTCommand.cc \
	OPNICKSCommand.cc \
	QUOTECommand.cc \
	REHASHCommand.cc \
	RELOADCommand.cc \
	SCORECommand.cc \
	SETCommand.cc \
	SETGROUPCommand.cc \
	SHUTDOWNCommand.cc \
	STATUSCommand.cc \
	SUSPENDCommand.cc \
	UNALERTCommand.cc \
	UNBLOCKCommand.cc \
	UNSUSPENDCommand.cc \
	USETCommand.cc \
	WHOGROUPCommand.cc \
	WHOISCommand.cc \
	sqlChanOp.cc \
	sqlChannel.cc \
	sqlManager.cc \
	sqlUser.cc

libchanfix_la_CXXFLAGS = -I$(PGSQL_INCLUDE) -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld \
	-DHAVE_NAMESPACE_STD -DHAVE_CXX_STRING_HEADER -DDLLIMPORT="" \
	$(threadLib)

libchanfix_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++ \
	-l@BOOST_THREAD_LIB@

EXTRA_DIST = chanfix.h \
	chanfix.example.conf.in \
	chanfixCommands.h \
	chanfix_config.h \
	chanfix_misc.h \
	responses.h \
	sqlChanOp.h \
	sqlChannel.h \
	sqlManager.h \
	sqlUser.h \
	Timer.h \
        $(COMMAND_CONF)

confdir=${prefix}/bin
conf_DATA = chanfix.example.conf \
        $(COMMAND_CONF)
