## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.10 2002/11/26 03:33:24 jeekay Exp $"

lib_LTLIBRARIES = libnickserv.la
libnickserv_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++

libnickserv_la_SOURCES = \
  Logger.cc              \
  netData.cc             \
  nickserv.cc            \
  sqlManager.cc          \
  sqlUser.cc             \
  Stats.cc               \
  INFOCommand.cc         \
  INVITECommand.cc       \
  MODUSERCommand.cc      \
  RECOVERCommand.cc      \
  REGISTERCommand.cc     \
  SETCommand.cc          \
  SHUTDOWNCommand.cc     \
  STATSCommand.cc        \
  WHOAMICommand.cc

INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)
