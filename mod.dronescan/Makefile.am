lib_LTLIBRARIES = libdronescan.la
libdronescan_la_LDFLAGS = -module -export-dynamic -L@PGSQL_LIB@ -lpq++

libdronescan_la_SOURCES = 	\
	dronescan.cc		\
	CHECKCommand.cc		\
	ABNORMALSTest.cc	\
	HASOPTest.cc		\
	RANGETest.cc

INCLUDES = -I@PGSQL_INCLUDE@ $(INCLTDL)
