## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.14 2003/08/04 22:57:59 dan_karrels Exp $

noinst_PROGRAMS = test_burst \
	test_mtrie_load \
	test_mtrie_perf_summary \
	test_mtrie_perf \
	test_mtrie \
	test_stringtokenizer \
	test_econfig \
	test_gThread \
	test_signal \
	test_match \
	test_bot \
	test_kick_transaction
LDADD = $(glibdir)/libgnuworld.la

test_burst_SOURCES = burst.cc
test_mtrie_SOURCES = mtrie.cc
test_mtrie_load_SOURCES = mtrie_load.cc
test_mtrie_perf_SOURCES = mtrie_perf.cc
test_mtrie_perf_summary_SOURCES = mtrie_perf_summary.cc
test_stringtokenizer_SOURCES = stringtokenizer.cc
test_econfig_SOURCES = econfig.cc
test_match_SOURCES = match.cc
test_bot_SOURCES = testbot.cc
test_kick_transaction_SOURCES = test_kick_transaction.cc

test_gThread_SOURCES = gThread.cc
test_gThread_LDFLAGS = $(glibdir)/libgnuworld.la @threadLib@

test_signal_SOURCES = signal.cc
test_signal_LDFLAGS = $(glibdir)/libgnuworld.la @threadLib@

EXTRA_DIST = testbot.h

