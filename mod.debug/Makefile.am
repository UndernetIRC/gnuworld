## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES += libdebug.la

libdebug_la_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier

libdebug_la_LDFLAGS = -module -export-dynamic

libdebug_la_SOURCES = \
	mod.debug/debug.cc \
	mod.debug/debug-commands.cc \
	mod.debug/debug-xclient.cc \
	mod.debug/USERINFOCommand.cc \
	mod.debug/CHANINFOCommand.cc \
	mod.debug/SERVERSCommand.cc \
	mod.debug/SERVERINFOCommand.cc \
	mod.debug/SHUTDOWNCommand.cc

EXTRA_DIST += \
	mod.debug/debug.h \
	mod.debug/debug-commands.h
