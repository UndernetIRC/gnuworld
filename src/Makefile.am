## Process this file with automake to produce Makefile.in
bin_PROGRAMS = gnuworld

gnuworld_LDADD = $(bindir)/libgnuworld.la
gnuworld_LDFLAGS = -dlopen $(bindir)/libgnuworld.la
gnuworld_SOURCES = Channel.cc \
	ChannelUser.cc \
	Network.cc \
	client.cc \
	iClient.cc \
	iServer.cc \
	ip.cc \
	main.cc \
	server.cc
INCLUDES = -I../lib
