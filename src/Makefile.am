## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.22 2003/07/10 00:58:25 dan_karrels Exp $

lib_LTLIBRARIES = libgnuworldcore.la
libgnuworldcore_la_SOURCES = Channel.cc \
	ChannelUser.cc \
	LoadClientTimerHandler.cc \
	Network.cc \
	ServerTimerHandlers.cc \
	UnloadClientTimerHandler.cc \
	client.cc \
	iClient.cc \
	iServer.cc \
	ip.cc \
	server.cc
libgnuworldcore_la_DEPENDENCIES = $(glibdir)/libgnuworld.la \
	$(LIBLTDL)
libgnuworldcore_la_LDFLAGS = $(glibdir)/libgnuworld.la \
	$(LIBLTDL) -export-dynamic

bin_PROGRAMS = gnuworld
gnuworld_SOURCES = main.cc

gnuworld_LDFLAGS = $(glibdir)/libgnuworld.la \
	$(LIBLTDL) libgnuworldcore.la -export-dynamic \
	$(threadLib)
gnuworld_DEPENDENCIES = $(glibdir)/libgnuworld.la \
	$(LIBLTDL) libgnuworldcore.la
gnuworld_CXXFLAGS = $(INCLTDL)
