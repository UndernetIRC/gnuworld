## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.31 2009/06/24 17:39:44 mrbean_ Exp $

lib_LTLIBRARIES = libgnuworldcore.la
libgnuworldcore_la_SOURCES = Channel.cc \
	ChannelUser.cc \
	LoadClientTimerHandler.cc \
	Network.cc \
	ServerTimerHandlers.cc \
	UnloadClientTimerHandler.cc \
	cidr.cc	\
	client.cc \
	iClient.cc \
	iServer.cc \
	ip.cc \
	nickUserCidr.cc \
	server.cc \
	server_connection.cc \
	server_events.cc \
	server_glines.cc \
	server_timers.cc
libgnuworldcore_la_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld
libgnuworldcore_la_DEPENDENCIES = $(LIBGNUWORLD) $(LIBLTDL)
libgnuworldcore_la_LDFLAGS = $(LIBGNUWORLD) $(LIBLTDL) -export-dynamic

bin_PROGRAMS = gnuworld
gnuworld_SOURCES = main.cc

gnuworld_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
gnuworld_LDFLAGS = libgnuworldcore.la $(LIBLTDL) -export-dynamic \
	$(threadLib) $(LIBGNUWORLD)
gnuworld_DEPENDENCIES = $(LIBGNUWORLD) $(LIBLTDL) libgnuworldcore.la
