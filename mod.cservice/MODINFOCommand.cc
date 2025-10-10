/**
 * MODINFOCommand.cc
 *
 * 27/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Modifies a user's 'Level' record in a particular channel.
 *
 * Caveats:
 * 1. In the rare case of somebody attempting to MODINFO a forced access
 * that doesn't exist in the database, then the commit() will fail.
 * This is fine, as the modified record doesn't really exist anyway.
 * Shouldn't really happen, as trying to MODINFO a forced access doesn't
 * make sense - adduser and then MODINFO that :)
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
 * $Id: MODINFOCommand.cc,v 1.24 2010/04/10 19:49:57 danielaustin Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

namespace gnuworld
{
using std::string ;

bool MODINFOCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}

string command = string_upper(st[2]);
if ((command != "ACCESS") && (command != "AUTOMODE") && (command != "INVITE") && (command != "TOTP"))
	{
	Usage(theClient);
	return true;
	}

if(command != "INVITE"  && command != "TOTP" && st.size() < 5 )
	{
	Usage(theClient);
	return true;
	}
/*
 *  Fetch the sqlUser record attached to this client. If there isn't one,
 *  they aren't logged in - tell them they should be.
 */

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false;
	}

if (command == "TOTP") {
	if(bot->getAdminAccessLevel(theUser) < adminlevel::modinfo) {
		bot->Notice(theClient,
               		bot->getResponse(theUser,
                        	language::insuf_access,
	                        string("Sorry, you have insufficient access to perform that command.")));
		return false;
	}
	sqlUser* modUser = bot->getUserRecord(st[1]);
	if(!modUser) {
		bot->Notice(theClient,"I can't find %s anywhere",st[1].c_str());
		return false;
	}	
	if(theUser == modUser) {
		bot->Notice(theClient,"Sorry, you can not disable your own TOTP setting");
		return false;
	}

	if(string_upper(st[3]) == "OFF") {
		if(modUser->getFlag(sqlUser::F_TOTP_ENABLED)) {
			modUser->removeFlag(sqlUser::F_TOTP_ENABLED);
			bot->sendAccountFlags(modUser);
			if(!modUser->commit(theClient)) {
				bot->Notice(theClient,"Failed to disable totp for %s",st[1].c_str());
				return false;
			}
			bot->Notice(theClient,"TOTP Authentication disabled for %s",st[1].c_str());
			return true;
		} 
		bot->Notice(theClient,"TOTP Authentication already disabled for %s",st[1].c_str());
		return false;
	} else if(string_upper(st[3]) == "ON") {
		bot->Notice(theClient,"Cannot enable TOTP for other users");
		return false;
	}
	bot->Notice(theClient,"Unknown option %s , valid option is OFF",st[3].c_str());
	return false;
}

if(st.size() < 4) {
	Usage(theClient);
	return true;
}
/*
 *  First, check the channel is registered.
 */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::chan_not_reg,
			string("Sorry, %s isn't registered with me.")).c_str(),
		st[1].c_str());
	return false;
	}

int required_level = (st[1] == "*") ? adminlevel::modinfo : level::modinfo;
if (command == "INVITE")
	{
	required_level = level::modinfo_autoinvite;
	}
/*
 *  Check the user has sufficient access on this channel.
 */
int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if (level < required_level)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
	return false;
	}

if(command == "INVITE")
	{ //Handle modinfo INVITE
	if (bot->getAccessLevel(theUser,theChan) == 0)
	{
		/* forced users cant set their invite status on, as they have no level record for the channel */
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::insuf_access,
				string("Sorry, you have insufficient access to perform that command.")));
		return false;
	}
	if(st[1] == "*")
	{ //Admin channel?
		bot->Notice(theClient,"It is a mistake to think you can solve any major problems just with potatoes.");
		return false;
	}
	if(string_upper(st[3]) == "ON")
		{ //Time to enable INVITE option
		sqlLevel* aLevel = bot->getLevelRecord(theUser, theChan);
		aLevel->setFlag(sqlLevel::F_AUTOINVITE);
		aLevel->commit();

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::automode_invite,
				string("Set INVITE on login %s for channel %s")).c_str(),
				"ON", theChan->getName().c_str());

		return false;
		}
	else if(string_upper(st[3]) == "OFF")
		{
		sqlLevel* aLevel = bot->getLevelRecord(theUser, theChan);
		aLevel->removeFlag(sqlLevel::F_AUTOINVITE);
		aLevel->commit();

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::automode_invite,
				string("Set INVITE on login %s for channel %s")).c_str(),
				"OFF",theChan->getName().c_str());

		return false;
		}
	Usage(theClient);
	return false;
	}

/*
 *  Check the person we're trying to change actually exists.
 */

sqlUser* targetUser = bot->getUserRecord(st[3]);
if (!targetUser)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::not_registered,
			string("Sorry, I don't know who %s is.")).c_str(), st[3].c_str());
	return false;
	}

/*
 *  Check this user really does have access on this channel.
 */

int targetLevel = bot->getAccessLevel(targetUser, theChan);
if (targetLevel == 0)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::doesnt_have_access,
			string("%s doesn't appear to have access in %s.")).c_str(),
		targetUser->getUserName().c_str(),
		theChan->getName().c_str());
	return false;
	}

/*
 *  Figure out what they're doing - ACCESS or AUTOOP.
 */

if (command == "ACCESS")
	{
	/*
	 * Check we aren't trying to change someone with access
	 * higher (or equal) than ours.
	 */

	if (level <= targetLevel)
		{
		/*
		 * Let forced users modify their own user records in channels to
		 * any setting.
		 */
		if (!bot->isForced(theChan, theUser))
			{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::mod_access_higher,
					string("Cannot modify a user with equal or higher access than your own.")));
			return false;
			}
		}

	/*
	 * Check we aren't trying to set someone's access higher
	 * than ours.
	 */
	int newAccess = atoi(st[4].c_str());

	if ((newAccess <= 0) || (newAccess > 999))
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::inval_access,
				string("Invalid access level.")));
		return false;
		}

	/*
	 * And finally, check they aren't trying to give someone
	 * higher access than them.
	 */

	if (level <= newAccess)
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::cant_give_higher,
				string("Cannot give a user higher or equal access to your own.")));
		return false;
		}

	sqlLevel* aLevel = bot->getLevelRecord(targetUser, theChan);
	aLevel->setAccess(newAccess);
	aLevel->setLastModif(bot->currentTime());
	aLevel->setLastModifBy( string( "(" + theUser->getUserName() + ") " +theClient->getNickUserHost() ) );
	/* When modinfo access, it's proper to forget about unsuspend reason */
	if ((aLevel->getSuspendExpire() == 0) && (aLevel->getSuspendReason() != ""))
		aLevel->setSuspendReason(string());
	if ((aLevel->getFlag(sqlLevel::F_AUTOVOICE)) && (newAccess < level::voice))
		aLevel->removeFlag(sqlLevel::F_AUTOVOICE);
	if ((aLevel->getFlag(sqlLevel::F_AUTOOP)) && (newAccess < level::op))
		aLevel->removeFlag(sqlLevel::F_AUTOOP);
	if ((theChan->getUserFlags() == 2) && (newAccess >= level::voice) && (!aLevel->getFlag(sqlLevel::F_AUTOVOICE)))
		aLevel->setFlag(sqlLevel::F_AUTOVOICE);
	if ((theChan->getUserFlags() == 1) && (newAccess >= level::op) && (!aLevel->getFlag(sqlLevel::F_AUTOOP)))
		aLevel->setFlag(sqlLevel::F_AUTOOP);

	aLevel->commit();

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::mod_access_to,
			string("Modified %s's access level on channel %s to %i")).c_str(),
		targetUser->getUserName().c_str(),
		theChan->getName().c_str(),
		newAccess);
	if (targetUser != theUser)
			bot->NoteAllAuthedClients(targetUser, bot->getResponse(targetUser,language::acc_modif,string("Your access on %s has been modified from %i to %i")).c_str(), theChan->getName().c_str(), targetLevel, newAccess);
	// Announce the manager about the new access change
	if (level < 500)
	{
		string theMessage = TokenStringsParams("%s modified %s's access on channel %s from %i to %i",
				theUser->getUserName().c_str(), targetUser->getUserName().c_str(), theChan->getName().c_str(),targetLevel, newAccess);
		bot->NoteChannelManager(theChan, theMessage.c_str());
	}
	} // if( command == "ACCESS" )

if (command == "AUTOMODE")
	{
	
	if(st[1] == "*")
	{ //Admin channel?
		bot->Notice(theClient,"If there's anything more important than my ego around, I want it caught and shot now.");
		return false;
	}

	/*
	 * Check we aren't trying to change someone with access higher
	 * than ours. Also, make sure we can't change someone else's MODINFO
	 * if they have the same access as we do.
	 */

	if ( (level < targetLevel) || ((level == targetLevel) && (targetUser != theUser)) )
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::mod_access_higher,
				string("Cannot modify a user with higher access than your own.")));
		return false;
		}


	/*
	 *  Check for "ON" or "OFF" and act accordingly.
	 */

	if (string_upper(st[4]) == "OP")
	{
		sqlLevel* aLevel = bot->getLevelRecord(targetUser, theChan);
		aLevel->removeFlag(sqlLevel::F_AUTOVOICE);
		aLevel->setFlag(sqlLevel::F_AUTOOP);
//		aLevel->setLastModif(bot->currentTime());
//		aLevel->setLastModifBy(theClient->getNickUserHost());
		aLevel->commit();

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::automode_op,
				string("Set AUTOMODE to OP for %s on channel %s")).c_str(),
			targetUser->getUserName().c_str(),
			theChan->getName().c_str());

		return false;
	}

	if (string_upper(st[4]) == "VOICE")
	{
		sqlLevel* aLevel = bot->getLevelRecord(targetUser, theChan);
		aLevel->removeFlag(sqlLevel::F_AUTOOP);
		aLevel->setFlag(sqlLevel::F_AUTOVOICE);
//		aLevel->setLastModif(bot->currentTime());
//		aLevel->setLastModifBy(theClient->getNickUserHost());
		aLevel->commit();

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::automode_voice,
				string("Set AUTOMODE to VOICE for %s on channel %s")).c_str(),
			targetUser->getUserName().c_str(),
			theChan->getName().c_str());
		return false;
	}

	if (string_upper(st[4]) == "NONE")
		{
		sqlLevel* aLevel = bot->getLevelRecord(targetUser, theChan);
		aLevel->removeFlag(sqlLevel::F_AUTOOP);
		aLevel->removeFlag(sqlLevel::F_AUTOVOICE);
//		aLevel->setLastModif(bot->currentTime());
//		aLevel->setLastModifBy(theClient->getNickUserHost());
		aLevel->commit();

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::automode_none,
				string("Set AUTOMODE to NONE for %s on channel %s")).c_str(),
			targetUser->getUserName().c_str(),
			theChan->getName().c_str());
		return false;
		}

	Usage(theClient);
	return true;
	}

return true ;
}

} // namespace gnuworld
