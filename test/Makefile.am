## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.16 2003/09/14 16:35:55 dan_karrels Exp $

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
LDADD = $(LIBGNUWORLD)

test_burst_SOURCES = burst.cc
test_burst_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_mtrie_SOURCES = mtrie.cc
test_mtrie_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_mtrie_load_SOURCES = mtrie_load.cc
test_mtrie_load_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_mtrie_perf_SOURCES = mtrie_perf.cc
test_mtrie_perf_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_mtrie_perf_summary_SOURCES = mtrie_perf_summary.cc
test_mtrie_perf_summary_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld

test_stringtokenizer_SOURCES = stringtokenizer.cc
test_stringtokenizer_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld

test_econfig_SOURCES = econfig.cc
test_econfig_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_match_SOURCES = match.cc
test_match_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_bot_SOURCES = testbot.cc
test_bot_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_kick_transaction_SOURCES = test_kick_transaction.cc
test_kick_transaction_CXXFLAGS = -I${top_srcdir}/include \
	-I${top_srcdir}/libgnuworld

test_gThread_SOURCES = gThread.cc
test_gThread_LDFLAGS = $(top_builddir)/libgnuworld/libgnuworld.la \
	$(threadLib)
test_gThread_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_signal_SOURCES = signal.cc
test_signal_LDFLAGS = $(LIBGNUWORLD) $(threadLib)
test_signal_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

EXTRA_DIST = testbot.h

