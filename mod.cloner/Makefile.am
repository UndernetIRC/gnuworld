## Process this file with automake to produce Makefile.in
# "$Id: Makefile.am,v 1.3 2000/12/19 21:32:35 dan_karrels Exp $"

lib_LTLIBRARIES = libcloner.la
libcloner_la_SOURCES = cloner.cc
libcloner_la_LDFLAGS = -module -export-dynamic \
	$(glibdir)/libgnuworld.la -R$(bindir)
