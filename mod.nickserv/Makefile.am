## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.7 2002/08/25 22:38:49 jeekay Exp $"

lib_LTLIBRARIES = libnickserv.la
libnickserv_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++

libnickserv_la_SOURCES = \
  netData.cc             \
  nickserv.cc            \
  sqlManager.cc          \
  sqlUser.cc             \
  Stats.cc               \
  RECOVERCommand.cc      \
  REGISTERCommand.cc     \
  SETCommand.cc          \
  STATSCommand.cc        \
  WHOAMICommand.cc

INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)
