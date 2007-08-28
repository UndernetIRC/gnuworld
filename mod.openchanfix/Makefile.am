## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.11 2007/08/28 16:10:21 dan_karrels Exp $"

lib_LTLIBRARIES = libchanfix.la

libchanfix_la_SOURCES = chanfix.cc \
	ADDFLAGCommand.cc \
	ADDHOSTCommand.cc \
	ADDNOTECommand.cc \
	ADDUSERCommand.cc \
	ALERTCommand.cc \
	BLOCKCommand.cc \
	CANFIXCommand.cc \
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
	LASTCOMCommand.cc \
	LISTBLOCKEDCommand.cc \
	LISTHOSTSCommand.cc \
	OPLISTCommand.cc \
	OPNICKSCommand.cc \
	QUOTECommand.cc \
	REHASHCommand.cc \
	RELOADCommand.cc \
	REQUESTOPCommand.cc \
	SCORECommand.cc \
	SAYCommand.cc \
	SETCommand.cc \
	SETGROUPCommand.cc \
	SHUTDOWNCommand.cc \
	SIMULATECommand.cc \
	STATUSCommand.cc \
	SUSPENDCommand.cc \
	UNALERTCommand.cc \
	UNBLOCKCommand.cc \
	UNSUSPENDCommand.cc \
	USERSCORESCommand.cc \
	USETCommand.cc \
	WHOFLAGCommand.cc \
	WHOGROUPCommand.cc \
	WHOISCommand.cc \
	sqlChanOp.cc \
	sqlChannel.cc \
	sqlManager.cc \
	sqlcfUser.cc

libchanfix_la_CXXFLAGS = -I$(top_srcdir)db -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld \
	$(threadLib)

libchanfix_la_LDFLAGS = -module -export-dynamic -L$(top_builddir)/db \
	-lgnuworldDB @BOOST_THREAD_LIB@

EXTRA_DIST = chanfix.h \
	chanfixCommands.h \
	chanfix_config.h \
	chanfix_misc.h \
	responses.h \
	sqlChanOp.h \
	sqlChannel.h \
	sqlManager.h \
	sqlcfUser.h \
        $(COMMAND_CONF)

