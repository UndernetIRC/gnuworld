## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.8 2003/06/08 20:06:43 dan_karrels Exp $

noinst_PROGRAMS = test_stringtokenizer \
	test_econfig \
	test_gThread \
	test_signal \
	test_match \
	test_bot
LDADD = $(glibdir)/libgnuworld.la

test_stringtokenizer_SOURCES = stringtokenizer.cc
test_stringtokenizer_DEPENDENCIES = $(glibdir)/libgnuworld.la

test_econfig_SOURCES = econfig.cc
test_econfig_DEPENDENCIES = $(glibdir)/libgnuworld.la

test_gThread_SOURCES = gThread.cc
test_gThread_LDFLAGS = $(glibdir)/libgnuworld.la @threadLib@
test_gThread_DEPENDENCIES = $(glibdir)/libgnuworld.la

test_signal_SOURCES = signal.cc
test_signal_LDFLAGS = $(glibdir)/libgnuworld.la @threadLib@
test_signal_DEPENDENCIES = $(glibdir)/libgnuworld.la

test_match_SOURCES = match.cc
test_match_DEPENDENCIES = $(glibdir)/libgnuworld.la

test_bot_SOURCES = testbot.cc
test_bot_DEPENDENCIES = $(glibdir)/libgnuworld.la

