## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.15 2001/12/23 09:07:57 mrbean_ Exp $

bin_PROGRAMS = gnuworld

gnuworld_LDFLAGS = -rpath $(bindir) $(glibdir)/libgnuworld.la \
	-R$(bindir) -export-dynamic -module $(LIBLTDL)
##gnuworld_LDADD = $(LIBLTDL) "-dlopen" self "-dlopen" \
##	$(glibdir)/libgnuworld.la
gnuworld_DEPENDENCIES = $(LIBLTDL) $(glibdir)/libgnuworld.la
gnuworld_SOURCES = Channel.cc \
	ChannelUser.cc \
	LoadClientTimerHandler.cc \
	Network.cc \
	ServerTimerHandlers.cc \
	UnloadClientTimerHandler.cc \
	client.cc \
	iClient.cc \
	iServer.cc \
	ip.cc \
	main.cc \
	msg_AD.cc \
	msg_B.cc \
	msg_C.cc \
	msg_CM.cc \
	msg_D.cc \
	msg_DS.cc \
	msg_EA.cc \
	msg_EB.cc \
	msg_Error.cc \
	msg_G.cc \
	msg_GL.cc \
	msg_I.cc \
	msg_J.cc \
	msg_JU.cc \
	msg_K.cc \
	msg_L.cc \
	msg_M.cc \
	msg_M351.cc \
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
INCLUDES = $(INCLTDL)
