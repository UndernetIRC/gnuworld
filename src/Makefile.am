## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.20 2002/12/02 00:34:09 dan_karrels Exp $

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
libgnuworldcore_la_LDFLAGS = -rpath $(libdir) $(glibdir)/libgnuworld.la \
	$(LIBLTDL) -export-dynamic

bin_PROGRAMS = gnuworld
gnuworld_SOURCES = main.cc

gnuworld_LDFLAGS = $(glibdir)/libgnuworld.la \
	$(LIBLTDL) libgnuworldcore.la -R$(bindir) -export-dynamic
gnuworld_DEPENDENCIES = $(glibdir)/libgnuworld.la \
	$(LIBLTDL) libgnuworldcore.la
INCLUDES = $(INCLTDL)
LIBS = @threadLib@ @LIBS@
