## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.4 2003/03/10 23:25:33 dan_karrels Exp $

bin_PROGRAMS = stringtokenizer econfig gThread signal

stringtokenizer_SOURCES = stringtokenizer.cc
stringtokenizer_LDFLAGS = $(glibdir)/libgnuworld.la
stringtokenizer_DEPENDENCIES = $(glibdir)/libgnuworld.la

econfig_SOURCES = econfig.cc
econfig_LDFLAGS = $(glibdir)/libgnuworld.la
econfig_DEPENDENCIES = $(glibdir)/libgnuworld.la

gThread_SOURCES = gThread.cc
gThread_LDFLAGS = $(glibdir)/libgnuworld.la @threadLib@
gThread_DEPENDENCIES = $(glibdir)/libgnuworld.la

signal_SOURCES = signal.cc
signal_LDFLAGS = $(glibdir)/libgnuworld.la @threadLib@
signal_DEPENDENCIES = $(glibdir)/libgnuworld.la
