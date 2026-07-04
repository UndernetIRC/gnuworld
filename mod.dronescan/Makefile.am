
lib_LTLIBRARIES += libdronescan.la

# Tie the module's soname to the current commit count so every new
# commit gets a distinct libdronescan.so.<N>. RELOAD unloads and
# reloads this module in-process via libtool's lt_dlopen/lt_dlclose;
# libltdl keeps its own process-wide cache of open modules keyed by
# resolved filename (ltdl.c's tryall_dlopen()), and if a prior
# unload ever leaves that cache entry behind (its ref_count not
# reaching zero, for whatever reason), the next lt_dlopen() silently
# reuses the stale, already-mapped handle instead of touching the
# freshly-installed .so on disk. A soname that changes with every
# commit means that stale-cache lookup can never match a new build,
# so a RELOAD after `git pull && make install` is guaranteed to load
# the new code regardless of whether the old handle was ever
# properly released.
DRONESCAN_BUILD_TAG := $(shell git -C $(top_srcdir) rev-list --count HEAD 2>/dev/null || echo 0)
libdronescan_la_LDFLAGS = -module -export-dynamic -version-info $(DRONESCAN_BUILD_TAG):0:0
libdronescan_la_LIBADD = libgnuworldDB.la -lpcre2-8
libdronescan_la_CXXFLAGS = -I$(top_srcdir)/db \
	-I${top_srcdir}/libgnuworld \
	-I${top_srcdir}/libnotifier \
	-I${top_srcdir}/include

libdronescan_la_SOURCES = 	\
	mod.dronescan/dronescan.cc		\
	mod.dronescan/Convert.cc		\
	mod.dronescan/sqlFakeClient.cc	\
	mod.dronescan/sqlUser.cc		\
	mod.dronescan/sqlSpamEvent.cc	\
	mod.dronescan/sqlSpamRule.cc	\
	mod.dronescan/sqlSpamAction.cc	\
	mod.dronescan/sqlSpamRuleAction.cc\
	mod.dronescan/sqlSpamExclusion.cc\
	mod.dronescan/sqlSpyClient.cc	\
	mod.dronescan/sqlMonitoredChannel.cc\
	mod.dronescan/ACCESSCommand.cc	\
	mod.dronescan/ADDEXCEPTIONALCHANNELCommand.cc\
	mod.dronescan/REMEXCEPTIONALCHANNELCommand.cc\
	mod.dronescan/RELOADCommand.cc\
	mod.dronescan/REHASHCommand.cc\
	mod.dronescan/ADDUSERCommand.cc	\
	mod.dronescan/ANALYSECommand.cc	\
	mod.dronescan/CHECKCommand.cc		\
	mod.dronescan/FAKECommand.cc		\
	mod.dronescan/HELLOCommand.cc		\
	mod.dronescan/HELPCommand.cc		\
	mod.dronescan/LISTCommand.cc		\
	mod.dronescan/MODUSERCommand.cc	\
	mod.dronescan/QUOTECommand.cc		\
	mod.dronescan/REMUSERCommand.cc	\
	mod.dronescan/SPAMCommand.cc		\
	mod.dronescan/STATUSCommand.cc	\
	mod.dronescan/ABNORMALSTest.cc	\
	mod.dronescan/COMMONREALTest.cc	\
	mod.dronescan/HASALLOPTest.cc		\
	mod.dronescan/HASOPTest.cc		\
	mod.dronescan/MAXCHANSTest.cc		\
	mod.dronescan/RANGETest.cc		\
	mod.dronescan/jfChannel.cc

EXTRA_DIST += \
	mod.dronescan/activeChannel.h \
	mod.dronescan/clientData.h \
	mod.dronescan/constants.h \
	mod.dronescan/Convert.h \
	mod.dronescan/dronescanCommands.h \
	mod.dronescan/dronescan.h \
	mod.dronescan/dronescanTests.h \
	mod.dronescan/levels.h \
	mod.dronescan/sqlFakeClient.h \
	mod.dronescan/sqlUser.h \
	mod.dronescan/sqlSpamEvent.h \
	mod.dronescan/sqlSpamRule.h \
	mod.dronescan/sqlSpamAction.h \
	mod.dronescan/sqlSpamRuleAction.h \
	mod.dronescan/sqlSpamExclusion.h \
	mod.dronescan/sqlSpyClient.h \
	mod.dronescan/sqlMonitoredChannel.h \
	mod.dronescan/ChangeLog \
	mod.dronescan/README \
	mod.dronescan/migrations/README
