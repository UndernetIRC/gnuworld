## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.17 2007/08/28 16:10:18 dan_karrels Exp $"

lib_LTLIBRARIES += libnickserv.la
libnickserv_la_LDFLAGS = -module -export-dynamic
libnickserv_la_LIBADD = libgnuworldDB.la

libnickserv_la_SOURCES = \
	mod.nickserv/INFOCommand.cc         \
	mod.nickserv/INVITECommand.cc       \
	mod.nickserv/Logger.cc              \
	mod.nickserv/MODUSERCommand.cc      \
	mod.nickserv/RECOVERCommand.cc      \
	mod.nickserv/REGISTERCommand.cc     \
	mod.nickserv/RELEASECommand.cc      \
	mod.nickserv/SETCommand.cc          \
	mod.nickserv/SHUTDOWNCommand.cc     \
	mod.nickserv/Stats.cc               \
	mod.nickserv/STATSCommand.cc        \
	mod.nickserv/WHOAMICommand.cc       \
	mod.nickserv/netData.cc             \
	mod.nickserv/nickserv.cc            \
	mod.nickserv/sqlManager.cc          \
	mod.nickserv/sqlUser.cc

libnickserv_la_CXXFLAGS = -I$(top_srcdir)/db \
	-I${top_srcdir}/include -I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier

EXTRA_DIST += \
	mod.nickserv/levels.h \
	mod.nickserv/Logger.h \
	mod.nickserv/logTarget.h \
	mod.nickserv/netData.h \
	mod.nickserv/nickservCommands.h \
	mod.nickserv/nickserv.h \
	mod.nickserv/responses.h \
	mod.nickserv/sqlManager.h \
	mod.nickserv/sqlUser.h \
	mod.nickserv/Stats.h \
	mod.nickserv/ChangeLog \
	mod.nickserv/Updates \
	mod.nickserv/README
