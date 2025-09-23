/**
 * VERIFYCommand.cc
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
 * $Id: VERIFYCommand.cc,v 1.24 2005/11/17 01:37:13 kewlio Exp $
 */
#include	<string>
#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

namespace gnuworld
{
using std::string ;

bool VERIFYCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.VERIFY");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

string extra;

sqlUser* tmpUser = bot->isAuthed(theClient, false);
iClient* target = Network->findNick(st[1]);
if(!target)
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::dont_see_them,
			string("Sorry, I don't see %s anywhere.")).c_str(), st[1].c_str());
	return false;
	}

if (target->getMode(iClient::MODE_SERVICES))
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::is_service_bot,
			string("%s is an Official Undernet Service Bot.")).c_str(),
		target->getNickName().c_str());
	return false;
	}

/*
 *  Firstly, deal with unauthenticated users.
 */

sqlUser* theUser = bot->isAuthed(target, false);

if (target->isOper())
	{
	extra = bot->getResponse(tmpUser,
		language::is_also_an_ircop, " and an IRC operator");
	}

if (!theUser)
	{
	if(target->isOper())
		{
		bot->Notice(theClient,
			bot->getResponse(tmpUser,
				language::is_an_ircop,
				string("%s is an IRC operator")).c_str(),
			target->getNickUserHost().c_str());
		}
	else
		{
		bot->Notice(theClient,
			bot->getResponse(tmpUser,
			language::is_not_logged_in,
			string("%s is NOT logged in.")).c_str(),
		target->getNickUserHost().c_str());
		}
		return false;
	}

sqlChannel* theChan = bot->getChannelRecord(bot->getConfcoderChan());
//if (!theChan)
//	{
//	return true;
//	}

// TODO: Move all the levels to constants in levels.h

int level = bot->getAdminAccessLevel(theUser, true);
int cLevel;
if (!theChan)
	cLevel = 0;
else
	cLevel = bot->getEffectiveAccessLevel(theUser, theChan, false);

cLevel = 0;
if ( (0 == level) && (0 == cLevel) )
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::logged_in_as,
			string("%s is logged in as %s%s")).c_str(),
		target->getNickUserHost().c_str(),
		theUser->getUserName().c_str(),
		extra.c_str());
	return false;
	}

if (level >= level::admin::base)
{
	if (theUser->getFlag(sqlUser::F_ALUMNI))
	{
	bot->Notice(theClient,
			bot->getResponse(tmpUser,
				language::is_cservice_alumni,
				string("%s is a member of the CService Alumni%s and logged in as %s")).c_str(),
			target->getNickUserHost().c_str(),
			extra.c_str(),
			theUser->getUserName().c_str());
	return true;
	}
}

if ((level >= level::admin::base) && (level <= level::admin::helper))
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::is_cservice_rep,
			string("%s is an Official CService Representative%s and logged in as %s")).c_str(),
		target->getNickUserHost().c_str(),
		extra.c_str(),
		theUser->getUserName().c_str());
	return true;
	}

if ((level > level::admin::helper) && (level <= level::admin::admin))
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::is_cservice_admin,
			string("%s is an Official CService Administrator%s and logged in as %s")).c_str(),
		target->getNickUserHost().c_str(),
		extra.c_str(),
		theUser->getUserName().c_str());
	return true;
	}

if ((level > level::admin::admin) && (level <= level::admin::coder))
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::is_cservice_dev,
			string("%s is an Official CService Developer%s and logged in as %s")).c_str(),
		target->getNickUserHost().c_str(),
		extra.c_str(),
		theUser->getUserName().c_str());
	return true;
	}

/*
if ((cLevel >= level::coder::base) && (cLevel <= level::coder::contrib))
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::is_coder_rep,
			string("%s is an Official Coder-Com Representative%s and logged in as %s")).c_str(),
		target->getNickUserHost().c_str(),
		extra.c_str(),
		theUser->getUserName().c_str());
	return true;
	}

if ((cLevel > level::coder::base) && (cLevel <= level::coder::contrib))
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::is_coder_contrib,
			string("%s is an Official Coder-Com Contributer%s and logged in as %s")).c_str(),
		target->getNickUserHost().c_str(),
		extra.c_str(),
		theUser->getUserName().c_str());
	return true;
	}

if ((cLevel > level::coder::contrib) && (cLevel <= level::coder::devel))
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::is_coder_devel,
			string("%s is an Official Coder-Com Developer%s and logged in as %s")).c_str(),
		target->getNickUserHost().c_str(),
		extra.c_str(),
		theUser->getUserName().c_str());
	return true;
	}

if (cLevel > level::coder::devel)
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::is_coder_senior,
			string("%s is an Official Coder-Com Senior%s and logged in as %s")).c_str(),
		target->getNickUserHost().c_str(),
		extra.c_str(),
		theUser->getUserName().c_str());
	return true;
	}
*/

return true ;
}

} // namespace gnuworld.
