#ifndef __LEVELS_H
#define __LEVELS_H "$Id: levels.h,v 1.13 2001/02/20 23:40:45 gte Exp $"

/* 
 * levels.h
 *
 * 20/12/2000 - Perry Lorier <perry@coders.net>
 * Initial Version.
 * 
 * Defines access level constants for use in command handlers. 
 *
 * $Id: levels.h,v 1.13 2001/02/20 23:40:45 gte Exp $
 */

namespace gnuworld {

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
  
  const int masskick = 200;
  const int status2 = 200; // Users can see the channel modes too
  
  const int adduser = 400;
  const int clearmode = 400;
  const int modinfo = 400;
  const int remuser = 400;

  const int join = 450;
  const int part = 450;
  const int setcmd = 450;

  const int logs = 501; // Level that logs are visible at 
  
  const int registercmd = 600;
  const int purge = 750; 
  const int remignore = 600;

  const int servnotice = 800;
  const int say = 800;

  // Debug Commands
  const int shutdown = 1000;
  const int quote = 1000; 
 
  namespace set {
    const int alwaysop = 450;
    const int userflag = 450;
    const int autotopic = 450;
    const int url = 450;
    const int massdeoppro = 500;
    const int keywords = 450;
    
    const int desc = 450;
    const int mode = 450;
    const int noop = 500;
    const int oponly = 500;
    const int strictop = 500;
    const int lang = 500;
    const int floodpro = 500;
    const int autojoin = 500;
    
    const int nopurge = 501;
    const int special = 750;
    const int noreg = 750;
    const int neverreg = 750;
    const int suspend = 501;
    const int tempman = 501;
    const int caution = 501;
    const int vacation = 501;
  }

  namespace immune {
    const int massdeop = 450;
    const int suspendop = 450; // Immune from op'ing a suspended user.
    const int floodpro = 501;
  }
 
  namespace admin {
	const int base = 1;
  	const int helper = 500;
	const int admin = 999;
	const int coder = 1000;
  }

 }
}

#endif 
