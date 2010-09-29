## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.9 2004/01/07 20:23:20 dan_karrels Exp $

noinst_LTLIBRARIES = libgnuworld.la
libgnuworld_la_SOURCES = Buffer.cc \
	Connection.cc \
	ConnectionHandler.cc \
	ConnectionManager.cc \
	EConfig.cc \
	ELog.cc \
	Signal.cc \
	StringTokenizer.cc \
	gThread.cc \
	match.cc \
	md5hash.cc \
	misc.cc
libgnuworld_la_LDFLAGS = $(threadLib)
libgnuworld_la_CXXFLAGS = -I${top_srcdir}/include

EXTRA_DIST = Buffer.h \
	Connection.h \
	ConnectionHandler.h \
	ConnectionManager.h \
	EConfig.h \
	ELog.h \
	gThread.h \
	ircd_chattr.h \
	match.h \
	match_table.h \
	md5hash.h \
	misc.h \
	MTrie.h \
	MTrie.cc \
	Signal.h \
	StringTokenizer.h \
	xparameters.h
