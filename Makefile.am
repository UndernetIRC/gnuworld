## $Id: Makefile.am,v 1.17 2003/09/02 01:37:05 dan_karrels Exp $
## test

@SET_MAKE@

if COND_MODCCONTROL
  MODCCONTROL = mod.ccontrol
endif

if COND_MODCLIENTEXAMPLE
  MODCLIENTEXAMPLE = mod.clientExample
endif

if COND_MODCLONER
  MODCLONER = mod.cloner
endif

if COND_MODCSERVICE
  MODCSERVICE = mod.cservice
endif

if COND_MODDRONESCAN
  MODDRONESCAN = mod.dronescan
endif

if COND_MODGNUTEST
  MODGNUTEST = mod.gnutest
endif

if COND_MODNICKSERV
  MODNICKSERV = mod.nickserv
endif

if COND_MODSCANNER
  MODSCANNER = mod.scanner
endif

if COND_MODSTATS
  MODSTATS = mod.stats
endif

if COND_TESTDIR
  TESTDIR = test
endif

if COND_DBDIR
  DBDIR = db
endif

SUBDIRS = libgnuworld libltdl src @NEW_MODS@ \
	$(MODCCONTROL) \
	$(MODCLIENTEXAMPLE) \
	$(MODCLONER) \
	$(MODCSERVICE) \
	$(MODDRONESCAN) \
	$(MODGNUTEST) \
	$(MODNICKSERV) \
	$(MODSCANNER) \
	$(MODSTATS) \
	$(TESTDIR) \
	$(DBDIR) \
	libircu

EXTRA_DIST = bin/ccontrol.example.conf.in \
	bin/clientExample.example.conf.in \
	bin/cloner.example.conf.in \
	bin/cservice.example.conf.in \
	bin/dronescan.example.conf.in \
	bin/gnutest.example.conf.in \
	bin/GNUWorld.example.conf.in \
	bin/nickserv.example.conf.in \
	bin/scanner.example.conf.in \
	bin/stats.example.conf.in \
	bin/server_command_map.in \
	contrib/00INDEX.TXT \
	contrib/chktrans.py \
	contrib/encrypt.sh \
	doc/README.pgsql \
	doc/README \
	doc/README.appjudge \
	doc/README.ccontrol \
	doc/README.cservice \
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
	doc/update.channels.sql \
	doc/update.delete.sql \
	doc/update.gfxcode.sql \
	doc/update.statistics.sql \
	doc/update_fraud_lists.sql \
	doc/update_themes.sql \
	doc/update_tzmove_l.sql \
	doc/update_tzmove_r.sql \
	doc/cmaster/website/faq.html \
	doc/cmaster/website/index.html \
	doc/cmaster/website/install.html \
	doc/cmaster/website/manual.html \
	doc/cmaster/website/web.html \
	doc/cmaster/cleanup_traffic.sql \
	doc/cmaster/enc_pass.sh \
	doc/cmaster/fixDOS.sh \
	doc/cmaster/Manager1K.sh \
	doc/cmaster/WipeUser.sh \
	doc/cmaster/appjudge \
	doc/cmaster/appjudge-config \
	doc/cmaster/cron-judge.in \
	doc/cmaster/cycle-judge \
	doc/idoc/index.php \
	doc/idoc/README \
	doc/nickserv/nickserv.sql \
	doc/nickserv/INSTALL \
	doc/nickserv/USAGE \
	db/gnuworldDB.h \
	db/pgsqlDB.h \
	include/defs.h \
	include/Channel.h \
	include/ChannelUser.h \
	include/Gline.h \
	include/LoadClientTimerHandler.h \
	include/Network.h \
	include/Numeric.h \
	include/ServerCommandHandler.h \
	include/ServerTimerHandlers.h \
	include/TimerHandler.h \
	include/UnloadClientTimerHandler.h \
	include/client.h \
	include/config.h \
	include/events.h \
	include/iClient.h \
	include/iServer.h \
	include/ip.h \
	include/moduleLoader.h \
	include/server.h \
	libltdl/acconfig.h \
	libltdl/ltdl.h \
	libltdl/config.h
