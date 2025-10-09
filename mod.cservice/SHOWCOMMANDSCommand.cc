/**
 * SHOWCOMMANDSCommand.cc
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
 * $Id: SHOWCOMMANDSCommand.cc,v 1.30 2009/06/09 15:40:29 mrbean_ Exp $
 */

#include	<string>
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

namespace gnuworld
{
using std::string ;

static const char* lvl_1000_cmds = "\002Level 1000\002: anything";
static const char* lvl_900_cmds = "\002Level  900\002: shutdown rehash";
static const char* lvl_800_cmds = "\002Level  800\002: say servnotice";
static const char* lvl_750_cmds = "\002Level  750\002: register purge removeall";
static const char* lvl_600_cmds = "\002Level  600\002: remignore addcomment scanhost scanuname scanemail";

static const char* lvl_500_cmds = "\002Level  500\002: part";
static const char* lvl_450_cmds = "\002Level  450\002: join set%s*";
static const char* lvl_400_cmds = "\002Level  400\002: adduser clearmode* modinfo+ remuser";
static const char* lvl_100_cmds = "\002Level  100\002: op deop mode invite suspend unsuspend";
static const char* lvl_75_cmds = "\002Level   75\002: ban* unban*";
static const char* lvl_50_cmds = "\002Level   50\002: kick%s topic";
static const char* lvl_42_cmds = "\002Level   42\002: PANIC";
static const char* lvl_25_cmds = "\002Level   25\002: voice devoice";
static const char* lvl_1_cmds = "\002Level    1\002: banlist status%s";
static const char* lvl_adm_cmds = "\002Level    *\002: force unforce stats";
static const char* lvl_0_cmds = "\002Level    0\002: access chaninfo info help* lbanlist login motd* newpass showcommands showignore verify suspendme";

#ifdef USE_OPERPARTJOIN
static const char* lvl_oper_cmds = "\002Level Oper\002: operjoin* operpart*";
#else
static const char* lvl_oper_cmds = "\002Level Oper\002: ";
#endif

bool SHOWCOMMANDSCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2)
	{
	bot->Notice(theClient, lvl_0_cmds);
	if(theClient->isOper())
		{
		bot->Notice(theClient, lvl_oper_cmds);
		}
//		bot->Notice(theClient, cmdFooter);
	return true;
	}

/*
 *  Fetch the sqlUser record attached to this client. If there isn't one,
 *  they aren't logged in - tell them they should be.
 */

sqlUser* theUser = bot->isAuthed(theClient, false);
if (!theUser)
	{
	bot->Notice(theClient, lvl_0_cmds);
	if(theClient->isOper())
		{
		bot->Notice(theClient, lvl_oper_cmds);
		}
//		bot->Notice(theClient, cmdFooter);
	return true;
	}

/*
 *  Check the channel is actually registered.
 */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_not_reg).c_str(),
		st[1].c_str());
	return false;
	}

/*
 *  Fetch the users access level.
 */

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
int admin = bot->getAdminAccessLevel(theUser);

if (level >= 1000) bot->Notice(theClient, lvl_1000_cmds);
if (level >= 900) bot->Notice(theClient, lvl_900_cmds);
if (level >= 800) bot->Notice(theClient, lvl_800_cmds);
if (level >= 750) bot->Notice(theClient, lvl_750_cmds);
if (level >= 600) bot->Notice(theClient, lvl_600_cmds);
if (level >= 500) bot->Notice(theClient, lvl_500_cmds);
if (level >= 450) bot->Notice(theClient, lvl_450_cmds,
	(level>=500) ? "+" : "");
if (level >= 400) bot->Notice(theClient, lvl_400_cmds);
if (level >= 100) bot->Notice(theClient, lvl_100_cmds);
if (level >= 75) bot->Notice(theClient,  lvl_75_cmds);
if (level >= 50) bot->Notice(theClient,  lvl_50_cmds,
		(level>=200) ? "+" : "");
if (level == 42) bot->Notice(theClient,  lvl_42_cmds);
if (level >= 25) bot->Notice(theClient,  lvl_25_cmds);
if (level >= 1) bot->Notice(theClient,   lvl_1_cmds,
		(level>=200||admin>0||theClient->isOper()) ? "+" : "");

if (admin >= level::force)
	{
	bot->Notice(theClient, lvl_adm_cmds);
	}

bot->Notice(theClient, lvl_0_cmds);
if (theClient->isOper())
	{
	bot->Notice(theClient, lvl_oper_cmds);
	}
//	bot->Notice(theClient, cmdFooter);

return true ;
}

} // namespace gnuworld
