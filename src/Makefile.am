## Process this file with automake to produce Makefile.in
bin_PROGRAMS = gnuworld

##gnuworld_LDADD = $(bindir)/libgnuworld.la
gnuworld_LDFLAGS = -rpath $(bindir) -dlopen $(glibdir)/libgnuworld.la \
	-R$(bindir) -export-dynamic -module
gnuworld_SOURCES = Channel.cc \
	ChannelUser.cc \
	Network.cc \
	ServerTimerHandlers.cc \
	client.cc \
	iClient.cc \
	iServer.cc \
	ip.cc \
	main.cc \
	msg_AD.cc \
	msg_B.cc \
	msg_C.cc \
	msg_D.cc \
	msg_DS.cc \
	msg_EA.cc \
	msg_EB.cc \
	msg_Error.cc \
	msg_G.cc \
	msg_GL.cc \
	msg_J.cc \
	msg_K.cc \
	msg_L.cc \
	msg_M.cc \
	msg_N.cc \
	msg_NOOP.cc \
	msg_P.cc \
	msg_PART.cc \
	msg_PASS.cc \
	msg_PRIVMSG.cc \
	msg_Q.cc \
	msg_R.cc \
	msg_RemPing.cc \
	msg_S.cc \
	msg_SQ.cc \
	msg_Server.cc \
	msg_T.cc \
	msg_W.cc \
	msg_WA.cc \
	server.cc
INCLUDES = -I../lib
