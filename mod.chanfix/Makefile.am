lib_LTLIBRARIES = libchanfix.la

libchanfix_la_CXXFLAGS =	-I${top_srcdir}/include	\
				-I${top_srcdir}/libgnuworld

libchanfix_la_LDFLAGS = 	-module -export-dynamic

libchanfix_la_SOURCES = 	chanfix.cc		\
				chanfix-misc.cc		\
				chanfix-xclient.cc

EXTRA_DIST =	chanfix.h
