/**
 * levels.h
 *
 * 20/12/2000 - Perry Lorier <perry@coders.net>
 * Initial Version.
 *
 * Defines access level constants for use in command handlers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: levels.h,v 1.32 2010/04/10 18:56:06 danielaustin Exp $
 */

#ifndef __LEVELS_H
#define __LEVELS_H "$Id: levels.h,v 1.32 2010/04/10 18:56:06 danielaustin Exp $"

namespace gnuworld {

 namespace adminlevel {
  const int adduser = 800;
  const int remuser = 800;
  const int modinfo = 800;
  const int suspend = 600;
  const int unsuspend = 800;
 }

 namespace level {
  const int access = 0;
  const int banlist = 0;
  const int chaninfo = 0;
  const int deauth = 0; // Depreciated?
  const int help = 0;
  const int lbanlist = 0;
  const int map = 0;
  const int motd = 0;

  const int status = 1;
  const int force = 1; // And by definition, unforce.
  const int modinfo_autoinvite = 1;
  
  const int voice = 25;
  const int devoice = 25;

  const int kick = 50;
  const int topic = 50;

  const int ban = 75;
  const int unban = 75;

  const int deop = 100;
  const int op = 100;
  const int invite = 100;
  const int suspend = 100;
  const int unsuspend = 100;
  const int mode = 100;

  const int masskick = 200;
  const int status2 = 200; // Users can see the channel modes too

  const int adduser = 400;
  const int clearmode = 400;
  const int modinfo = 400;
  const int remuser = 400;

  const int join = 450;
  const int part = 500;
  const int setcmd = 450;

  const int logs = 501; // Level that logs are visible at

  const int addcommentcmd = 600;
  const int scanuname = 600;
  const int scanhost = 600;
  const int scanemail = 600;
  const int whitelist = 750;
  const int whitelistadmin = 800;
  const int registercmd = 750;
  const int globalsuspend = 750;
  const int rehash_admin = 800;
  const int rehash_coder = 900;

  const int purge = 750;
  const int removeall = 750;

  const int remignore = 600;

  const int servnotice = 800;
  const int say = 800;
  const int stats = 800;

  // Debug Commands
  const int shutdown = 900;
  const int quote = 1000;

  namespace set {
    const int alwaysop = 450;
    const int userflag = 450;
    const int autotopic = 450;
    const int url = 450;
    const int massdeoppro = 500;
    const int keywords = 450;
    const int floatlim = 450;

    const int desc = 450;
    const int mode = 450;
	const int welcome = 450;
    const int notake = 500;
    const int novoice = 450;
    const int noop = 500;
    const int oponly = 500;
    const int strictop = 500;
    const int lang = 500;
	const int oplog = 500;
    const int floodpro = 450;
    const int floodpro_gline = 800;
    const int autojoin = 500;

    const int nopurge = 501;
	const int comment = 600;
	const int locked = 600;
    const int special = 750;
    const int noreg = 750;
    const int neverreg = 750;
    const int suspend = 600;
    const int tempman = 501;
    const int caution = 501;
    const int vacation = 501;
    const int mia = 800;
	const int noforce = 1000;
  }

  namespace immune {
    const int massdeop = 450;
    const int suspendop = 450; // Immune from op'ing a suspended user.
    const int floodpro = 501;
	const int noforce = 1000;
  }

  namespace admin {
	const int base = 1;
  	const int helper = 500;
	const int admin = 949;
	const int coder = 1000;
  }

  namespace coder {
	const int base = 1;
	const int contrib = 200;
	const int devel = 400;
	const int senior = 499;
  }

 }
}

#endif
