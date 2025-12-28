## $Id: Makefile.am,v 1.5 2005/01/17 23:09:17 dan_karrels Exp $

lib_LTLIBRARIES += libchanfix.la

libchanfix_la_CXXFLAGS =	-I${top_srcdir}/include		\
				-I${top_srcdir}/libgnuworld	\
				-I${top_srcdir}/libnotifier	\
				-Werror

libchanfix_la_LDFLAGS = 	-module -export-dynamic

libchanfix_la_SOURCES = \
	mod.chanfix/chanfix.cc		\
	mod.chanfix/chanfix-commands.cc	\
	mod.chanfix/chanfix-core.cc		\
	mod.chanfix/chanfix-misc.cc		\
	mod.chanfix/chanfix-xclient.cc	\
	mod.chanfix/cfChannel.cc		\
	mod.chanfix/cfChannelUser.cc	\
	mod.chanfix/cmdCHECK.cc

EXTRA_DIST += \
	mod.chanfix/chanfix.h \
	mod.chanfix/cfChannel.h \
	mod.chanfix/cfChannelUser.h \
	mod.chanfix/chanfix-commands.h \
	mod.chanfix/logging.h
