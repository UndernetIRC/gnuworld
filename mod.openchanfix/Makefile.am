## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.11 2007/08/28 16:10:21 dan_karrels Exp $"

lib_LTLIBRARIES += libchanfix.la

libchanfix_la_SOURCES = \
	mod.openchanfix/chanfix.cc \
	mod.openchanfix/ADDFLAGCommand.cc \
	mod.openchanfix/ADDHOSTCommand.cc \
	mod.openchanfix/ADDNOTECommand.cc \
	mod.openchanfix/ADDUSERCommand.cc \
	mod.openchanfix/ALERTCommand.cc \
	mod.openchanfix/BLOCKCommand.cc \
	mod.openchanfix/CANFIXCommand.cc \
	mod.openchanfix/CHANFIXCommand.cc \
	mod.openchanfix/CHECKCommand.cc \
	mod.openchanfix/DEBUGCommand.cc \
	mod.openchanfix/DELFLAGCommand.cc \
	mod.openchanfix/DELHOSTCommand.cc \
	mod.openchanfix/DELNOTECommand.cc \
	mod.openchanfix/DELUSERCommand.cc \
	mod.openchanfix/HELPCommand.cc \
	mod.openchanfix/HISTORYCommand.cc \
	mod.openchanfix/INFOCommand.cc \
	mod.openchanfix/INVITECommand.cc \
	mod.openchanfix/LASTCOMCommand.cc \
	mod.openchanfix/LISTBLOCKEDCommand.cc \
	mod.openchanfix/LISTHOSTSCommand.cc \
	mod.openchanfix/OPLISTCommand.cc \
	mod.openchanfix/OPNICKSCommand.cc \
	mod.openchanfix/QUOTECommand.cc \
	mod.openchanfix/REHASHCommand.cc \
	mod.openchanfix/RELOADCommand.cc \
	mod.openchanfix/REQUESTOPCommand.cc \
	mod.openchanfix/SCORECommand.cc \
	mod.openchanfix/SAYCommand.cc \
	mod.openchanfix/SETCommand.cc \
	mod.openchanfix/SETGROUPCommand.cc \
	mod.openchanfix/SHUTDOWNCommand.cc \
	mod.openchanfix/SIMULATECommand.cc \
	mod.openchanfix/STATUSCommand.cc \
	mod.openchanfix/SUSPENDCommand.cc \
	mod.openchanfix/UNALERTCommand.cc \
	mod.openchanfix/UNBLOCKCommand.cc \
	mod.openchanfix/UNSUSPENDCommand.cc \
	mod.openchanfix/USERSCORESCommand.cc \
	mod.openchanfix/USETCommand.cc \
	mod.openchanfix/WHOFLAGCommand.cc \
	mod.openchanfix/WHOGROUPCommand.cc \
	mod.openchanfix/WHOISCommand.cc \
	mod.openchanfix/sqlChanOp.cc \
	mod.openchanfix/sqlChannel.cc \
	mod.openchanfix/sqlManager.cc \
	mod.openchanfix/sqlcfUser.cc

libchanfix_la_CXXFLAGS = -I$(top_srcdir)db -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier \
	$(threadLib)
libchanfix_la_LDFLAGS = -module -export-dynamic @BOOST_THREAD_LIB@
libchanfix_la_LIBADD = libgnuworldDB.la

EXTRA_DIST += \
	mod.openchanfix/chanfix.h \
	mod.openchanfix/chanfixCommands.h \
	mod.openchanfix/chanfix_config.h \
	mod.openchanfix/chanfix_misc.h \
	mod.openchanfix/responses.h \
	mod.openchanfix/sqlChanOp.h \
	mod.openchanfix/sqlChannel.h \
	mod.openchanfix/sqlManager.h \
	mod.openchanfix/sqlcfUser.h

