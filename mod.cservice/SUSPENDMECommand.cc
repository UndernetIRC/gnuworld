/**
 * SUSPENDMECommand.cc
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
 * $Id: SUSPENDMECommand.cc,v 1.4 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>
#include	<ctime>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

namespace gnuworld
{
using std::string ;
using namespace level;

bool SUSPENDMECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false;
	}

/*
 * Check password, if its wrong, bye bye.
 */

if (!bot->isPasswordRight(theUser, st.assemble(1)))
	{
	bot->Notice(theClient, "Self-suspension failed.");
	return false;
	}

if (theUser->networkClientList.size() <= 1)
	{
	bot->Notice(theClient, "Self-suspension failed.");
	return false;
	}

if(theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
	bot->Notice(theClient, "Self-suspension failed.");
	return false;
	}

// Suspend them.
theUser->setFlag(sqlUser::F_GLOBAL_SUSPEND);
bot->sendAccountFlags(theUser);
theUser->commit(theClient);
bot->Notice(theClient, "You have been globally suspended and will have level 0 access in all"
	" channels until you are unsuspended by a CService administrator.");

theUser->writeEvent(sqlUser::EV_SUSPEND, theUser, "Self-Suspension");

bot->logAdminMessage("%s (%s) has globally suspended their own account.",
	theClient->getNickUserHost().c_str(), theUser->getUserName().c_str());

bot->InsertUserHistory(theClient, "SUSPENDME");

return false;
}

} // namespace gnuworld.
