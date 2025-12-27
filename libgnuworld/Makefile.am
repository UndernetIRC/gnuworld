## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.9 2004/01/07 20:23:20 dan_karrels Exp $

noinst_LTLIBRARIES += libgnuworld.la
libgnuworld_la_SOURCES = \
	libgnuworld/Buffer.cc \
	libgnuworld/Connection.cc \
	libgnuworld/ConnectionHandler.cc \
	libgnuworld/ConnectionManager.cc \
	libgnuworld/EConfig.cc \
	libgnuworld/ELog.cc \
	libgnuworld/Signal.cc \
	libgnuworld/StringTokenizer.cc \
	libgnuworld/gThread.cc \
	libgnuworld/match.cc \
	libgnuworld/md5hash.cc \
	libgnuworld/threadworker.cc \
	libgnuworld/misc.cc

libgnuworld_la_LDFLAGS = $(threadLib)

libgnuworld_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/libgnuworld

EXTRA_DIST += \
	libgnuworld/Buffer.h \
	libgnuworld/Connection.h \
	libgnuworld/ConnectionHandler.h \
	libgnuworld/ConnectionManager.h \
	libgnuworld/EConfig.h \
	libgnuworld/ELog.h \
	libgnuworld/gThread.h \
	libgnuworld/ircd_chattr.h \
	libgnuworld/match.h \
	libgnuworld/match_table.h \
	libgnuworld/md5hash.h \
	libgnuworld/threadworker.h \
	libgnuworld/misc.h \
	libgnuworld/MTrie.h \
	libgnuworld/MTrie.cc \
	libgnuworld/Signal.h \
	libgnuworld/StringTokenizer.h \
	libgnuworld/xparameters.h
