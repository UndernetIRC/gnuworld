## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.11 2003/06/28 01:21:21 dan_karrels Exp $"

lib_LTLIBRARIES = libnickserv.la
libnickserv_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++

libnickserv_la_SOURCES = \
  INFOCommand.cc         \
  INVITECommand.cc       \
  Logger.cc              \
  MODUSERCommand.cc      \
  RECOVERCommand.cc      \
  REGISTERCommand.cc     \
  SETCommand.cc          \
  SHUTDOWNCommand.cc     \
  Stats.cc		 \
  STATSCommand.cc        \
  WHOAMICommand.cc	 \
  netData.cc             \
  nickserv.cc            \
  sqlManager.cc          \
  sqlUser.cc

INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)

EXTRA_DIST = levels.h \
	Logger.h \
	logTarget.h \
	netData.h \
	nickservCommands.h \
	nickserv.h \
	responses.h \
	sqlManager.h \
	sqlUser.h \
	Stats.h \
	ChangeLog \
	Updates \
	README
