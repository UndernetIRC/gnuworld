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
LDADD = $(LIBGNUWORLD)

if COND_PCRE
  noinst_PROGRAMS += test_regex
endif

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

test_regex_SOURCES = test_regex.cc
test_regex_LDFLAGS = -lpcre

test_gThread_SOURCES = gThread.cc
test_gThread_LDFLAGS = $(top_builddir)/libgnuworld/libgnuworld.la \
	$(threadLib)
test_gThread_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

test_signal_SOURCES = signal.cc
test_signal_LDFLAGS = $(LIBGNUWORLD) $(threadLib)
test_signal_CXXFLAGS = -I${top_srcdir}/include -I${top_srcdir}/libgnuworld

EXTRA_DIST = testbot.h \
	README.burst \
	README.econfig \
	README.gthread \
	README.match \
	README.mtrie \
	README.signal \
	README.stringtokenizer \
	README.testBot \
	README.test_kick_transaction
