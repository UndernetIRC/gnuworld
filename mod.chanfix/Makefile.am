lib_LTLIBRARIES = libchanfix.la

libchanfix_la_CXXFLAGS =	-I${top_srcdir}/include		\
				-I${top_srcdir}/libgnuworld	\
				-Werror -ansi -pedantic

libchanfix_la_LDFLAGS = 	-module -export-dynamic

libchanfix_la_SOURCES = 	chanfix.cc		\
				chanfix-core.cc		\
				chanfix-misc.cc		\
				chanfix-xclient.cc	\
				cfChannel.cc		\
				cfChannelUser.cc

EXTRA_DIST =	chanfix.h
