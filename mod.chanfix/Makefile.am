## $Id: Makefile.am,v 1.5 2005/01/17 23:09:17 dan_karrels Exp $

lib_LTLIBRARIES = libchanfix.la

libchanfix_la_CXXFLAGS =	-I${top_srcdir}/include		\
				-I${top_srcdir}/libgnuworld	\
				-Werror -ansi -pedantic

libchanfix_la_LDFLAGS = 	-module -export-dynamic

libchanfix_la_SOURCES = 	chanfix.cc		\
				chanfix-commands.cc	\
				chanfix-core.cc		\
				chanfix-misc.cc		\
				chanfix-xclient.cc	\
				cfChannel.cc		\
				cfChannelUser.cc	\
				cmdCHECK.cc

EXTRA_DIST =	chanfix.h \
		cfChannel.h \
		cfChannelUser.h \
		chanfix-commands.h \
		logging.h
