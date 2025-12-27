## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.31 2009/06/24 17:39:44 mrbean_ Exp $

lib_LTLIBRARIES += libgnuworldcore.la
libgnuworldcore_la_SOURCES = \
	src/Channel.cc \
	src/ChannelUser.cc \
	src/LoadClientTimerHandler.cc \
	src/Network.cc \
	src/ServerTimerHandlers.cc \
	src/UnloadClientTimerHandler.cc \
	src/client.cc \
	src/iClient.cc \
	src/iServer.cc \
	src/ip.cc \
	src/server.cc \
	src/server_connection.cc \
	src/server_events.cc \
	src/server_glines.cc \
	src/server_timers.cc
libgnuworldcore_la_CXXFLAGS =   -I${top_srcdir}/include \
				-I${top_srcdir}/libgnuworld \
				-I${top_srcdir}/libnotifier
libgnuworldcore_la_LDFLAGS = -export-dynamic
libgnuworldcore_la_LIBADD = libgnuworld.la libnotifier.la $(LIBLTDL)

bin_PROGRAMS = gnuworld
gnuworld_SOURCES = src/main.cc

gnuworld_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
gnuworld_LDFLAGS = -export-dynamic $(threadLib)
gnuworld_LDADD = libgnuworld.la libgnuworldcore.la $(LIBLTDL)
