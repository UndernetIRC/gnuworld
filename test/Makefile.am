## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.7 2003/06/03 01:01:56 dan_karrels Exp $

noinst_PROGRAMS = stringtokenizer econfig gThread signal match testbot
LDADD = $(glibdir)/libgnuworld.la

stringtokenizer_SOURCES = stringtokenizer.cc
stringtokenizer_DEPENDENCIES = $(glibdir)/libgnuworld.la

econfig_SOURCES = econfig.cc
econfig_DEPENDENCIES = $(glibdir)/libgnuworld.la

gThread_SOURCES = gThread.cc
gThread_LDFLAGS = $(glibdir)/libgnuworld.la @threadLib@
gThread_DEPENDENCIES = $(glibdir)/libgnuworld.la

signal_SOURCES = signal.cc
signal_LDFLAGS = $(glibdir)/libgnuworld.la @threadLib@
signal_DEPENDENCIES = $(glibdir)/libgnuworld.la

match_SOURCES = match.cc
match_DEPENDENCIES = $(glibdir)/libgnuworld.la

testbot_SOURCES = testbot.cc
testbot_DEPENDENCIES = $(glibdir)/libgnuworld.la

