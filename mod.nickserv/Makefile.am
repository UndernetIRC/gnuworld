## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.6 2002/08/25 16:21:44 jeekay Exp $"

lib_LTLIBRARIES = libnickserv.la
libnickserv_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++

libnickserv_la_SOURCES = \
  netData.cc             \
  nickserv.cc            \
  sqlManager.cc          \
  sqlUser.cc             \
  RECOVERCommand.cc      \
  REGISTERCommand.cc     \
  SETCommand.cc          \
  WHOAMICommand.cc

INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)
