## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.19 2007/09/15 18:03:34 dan_karrels Exp $

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

if COND_PCRE
noinst_PROGRAMS += test_regex
endif

test_burst_SOURCES = test/burst.cc
test_burst_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
test_burst_LDADD = libgnuworld.la

test_mtrie_SOURCES = test/mtrie.cc
test_mtrie_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
test_mtrie_LDADD = libgnuworld.la

test_mtrie_load_SOURCES = test/mtrie_load.cc
test_mtrie_load_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
test_mtrie_load_LDADD = libgnuworld.la

test_mtrie_perf_SOURCES = test/mtrie_perf.cc
test_mtrie_perf_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
test_mtrie_perf_LDADD = libgnuworld.la

test_mtrie_perf_summary_SOURCES = test/mtrie_perf_summary.cc
test_mtrie_perf_summary_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld
test_mtrie_perf_summary_LDADD = libgnuworld.la

test_stringtokenizer_SOURCES = test/stringtokenizer.cc
test_stringtokenizer_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld
test_stringtokenizer_LDADD = libgnuworld.la

test_econfig_SOURCES = test/econfig.cc
test_econfig_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
test_econfig_LDADD = libgnuworld.la

test_match_SOURCES = test/match.cc
test_match_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
test_match_LDADD = libgnuworld.la

test_bot_SOURCES = test/testbot.cc
test_bot_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld
test_bot_LDADD = libgnuworld.la

test_kick_transaction_SOURCES = test/test_kick_transaction.cc
test_kick_transaction_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld
test_kick_transaction_LDADD = libgnuworld.la

test_regex_SOURCES = test/test_regex.cc
test_regex_LDADD = libgnuworld.la -lpcre

test_gThread_SOURCES = test/gThread.cc
test_gThread_LDADD = libgnuworld.la $(threadLib)
test_gThread_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_signal_SOURCES = test/signal.cc
test_signal_LDADD = libgnuworld.la $(threadLib)
test_signal_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

EXTRA_DIST += \
	test/testbot.h \
	test/README.burst \
	test/README.econfig \
	test/README.gthread \
	test/README.match \
	test/README.mtrie \
	test/README.signal \
	test/README.stringtokenizer \
	test/README.testBot \
	test/README.test_kick_transaction
