## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.25 2003/09/02 00:54:58 jeekay Exp $

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
libgnuworldcore_la_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
libgnuworldcore_la_DEPENDENCIES = $(top_builddir)/libgnuworld/libgnuworld.la \
	$(LIBLTDL)
libgnuworldcore_la_LDFLAGS = $(top_builddir)/libgnuworld/libgnuworld.la \
	$(LIBLTDL) -export-dynamic

bin_PROGRAMS = gnuworld
gnuworld_SOURCES = main.cc

gnuworld_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
gnuworld_LDFLAGS = $(top_builddir)/libgnuworld/libgnuworld.la \
	$(LIBLTDL) libgnuworldcore.la -export-dynamic \
	$(threadLib)
gnuworld_DEPENDENCIES = $(top_builddir)/libgnuworld/libgnuworld.la \
	$(LIBLTDL) libgnuworldcore.la
