## $Id: Makefile.am,v 1.26 2007/08/28 16:09:56 dan_karrels Exp $
## test

@SET_MAKE@

SUBDIRS = libltdl $(NEW_MODS)
lib_LTLIBRARIES =
noinst_LTLIBRARIES =

pkgdata_DATA = bin/server_command_map

EXTRA_DIST = bin/ccontrol.example.conf \
	bin/chanfix.example.conf \
	bin/clientExample.example.conf \
	bin/cloner.example.conf \
	bin/cservice.example.conf \
	bin/dronescan.example.conf \
	bin/gnutest.example.conf \
	bin/GNUWorld.example.conf \
	bin/nickserv.example.conf \
	bin/openchanfix.example.conf \
	bin/scanner.example.conf \
	bin/stats.example.conf \
	contrib/00INDEX.TXT \
	contrib/chktrans.py \
	contrib/encrypt.sh \
	doc/README.pgsql \
	doc/README \
	doc/README.appjudge \
	doc/README.ccontrol \
	doc/README.cservice \
	doc/README.dronescan \
	doc/README.sharedmemory \
	doc/autokill.sql \
	doc/ccontrol.addme.sql \
	doc/ccontrol.commands.sql \
	doc/ccontrol.help.sql \
	doc/ccontrol.sql \
	doc/ccontrol.update.sql \
	doc/checkvarargs.py \
	doc/chktrans.py \
	doc/cparse.py \
	doc/cservice.addme.sql \
	doc/cservice.config.sql \
	doc/cservice.help.sql \
	doc/cservice.sql \
	doc/cservice.web.sql \
	doc/dronescan.sql \
	doc/genhelp.py \
	doc/grant.sql \
	doc/greeting.sql \
	doc/language_christmas.sql \
	doc/language_dutch.sql \
	doc/language_easter.sql \
	doc/language_german.sql \
	doc/language_greek.sql \
	doc/language_halloween.sql \
	doc/language_swedish.sql \
	doc/language_table.sql \
	doc/languages.sql \
	doc/local_db.sql \
	doc/motd.sql \
	doc/movetables \
	doc/notes.sql \
	doc/README \
	doc/README.appjudge \
	doc/README.ccontrol \
	doc/README.cservice \
	doc/README.pgsql \
	doc/README.sharedmemory \
	doc/update.channels.sql \
	doc/update_complaints_20031020.sql \
	doc/update_complaints_20031026.sql \
	doc/update_complaints.sql \
	doc/update.delete.sql \
	doc/update.gfxcode.sql \
	doc/update.statistics.sql \
	doc/update_fraud_lists.sql \
	doc/update.statistics.sql \
	doc/update_themes.sql \
	doc/update_tzmove_l.sql \
	doc/update_tzmove_r.sql \
	doc/cmaster/maint/cleanup_traffic.sql \
	doc/cmaster/maint/enc_pass.sh \
	doc/cmaster/maint/fixDOS.sh \
	doc/cmaster/maint/Manager1K.sh \
	doc/cmaster/maint/WipeUser.sh \
	doc/cmaster/regproc/appjudge \
	doc/cmaster/regproc/appjudge-config \
	doc/cmaster/regproc/cron-judge.in \
	doc/cmaster/regproc/cycle-judge \
	doc/cmaster/website/faq.html \
	doc/cmaster/website/index.html \
	doc/cmaster/website/install.html \
	doc/cmaster/website/manual.html \
	doc/cmaster/website/web.html \
	doc/idoc/index.php \
	doc/idoc/README \
	doc/nickserv/nickserv.sql \
	doc/nickserv/INSTALL \
	doc/nickserv/USAGE \
	include/defs.h \
	include/Channel.h \
	include/ChannelUser.h \
	include/Gline.h \
	include/LoadClientTimerHandler.h \
	include/Network.h \
	include/NetworkTarget.h \
	include/Numeric.h \
	include/ServerCommandHandler.h \
	include/ServerTimerHandlers.h \
	include/Timer.h \
	include/TimerHandler.h \
	include/UnloadClientTimerHandler.h \
	include/client.h \
	include/events.h \
	include/iClient.h \
	include/iServer.h \
	include/ip.h \
	include/gnuworld_config.h \
	include/gw_hashmap.h \
	include/moduleLoader.h \
	include/server.h \
	libltdl/acconfig.h \
	libltdl/ltdl.h \
	libltdl/config.h \
	tools/checkgnuworld.sh

include db/Makefile.am
include libgnuworld/Makefile.am
include libnotifier/Makefile.am
include libircu/Makefile.am
include src/Makefile.am

if COND_MODCCONTROL
include $(srcdir)/mod.ccontrol/Makefile.am
endif

if COND_MODCHANFIX
include $(srcdir)/mod.chanfix/Makefile.am
endif

if COND_MODCLIENTEXAMPLE
include $(srcdir)/mod.clientExample/Makefile.am
endif

if COND_MODCLONER
include $(srcdir)/mod.cloner/Makefile.am
endif

if COND_MODCSERVICE
include $(srcdir)/mod.cservice/Makefile.am
endif

if COND_MODDRONESCAN
include $(srcdir)/mod.dronescan/Makefile.am
endif

if COND_MODGNUTEST
include $(srcdir)/mod.gnutest/Makefile.am
endif

if COND_MODNICKSERV
include $(srcdir)/mod.nickserv/Makefile.am
endif

if COND_MODOPENCHANFIX
include $(srcdir)/mod.openchanfix/Makefile.am
endif

if COND_MODSCANNER
include $(srcdir)/mod.scanner/Makefile.am
endif

if COND_MODSTATS
include $(srcdir)/mod.stats/Makefile.am
endif

if COND_MODSNOOP
include $(srcdir)/mod.snoop/Makefile.am
endif

if COND_TESTDIR
include $(srcdir)/test/Makefile.am
endif
