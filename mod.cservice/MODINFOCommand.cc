/* 
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
 * $Id: MODINFOCommand.cc,v 1.14 2001/03/07 23:22:11 dan_karrels Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"

const char MODINFOCommand_cc_rcsId[] = "$Id: MODINFOCommand.cc,v 1.14 2001/03/07 23:22:11 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
 
bool MODINFOCommand::Exec( iClient* theClient, const string& Message )
{ 
StringTokenizer st( Message ) ;
if( st.size() < 5 )
	{
	Usage(theClient);
	return true;
	}
 
const string command = string_upper(st[2]); 
if ((command != "ACCESS") && (command != "AUTOMODE")) 
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

/*
 *  Check the user has sufficient access on this channel. 
 */ 
int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if (level < level::modinfo)
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser, 
			language::insuf_access, 
			string("Sorry, you have insufficient access to perform that command.")));
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

/* Pointer to the requesting user's Level record */
sqlLevel* tmpLevel = bot->getLevelRecord(theUser, theChan);

if (command == "ACCESS")
	{ 
	/*
	 * Check we aren't trying to change someone with access
	 * higher (or equal) than ours.
	 */
	
	if (level <= targetLevel)
		{
		/* If the access is forced, they are allowed to
		 * modify their own record.
		 */
		if (!tmpLevel->getFlag(sqlLevel::F_FORCED))
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

	// Only commit changes if this has been loaded from the Db.
	// (Ie: If its a forced temporary access, this flag won't be set)..
	if (aLevel->getFlag(sqlLevel::F_ONDB)) aLevel->commit();
	bot->Notice(theClient, 
		bot->getResponse(theUser, 
			language::mod_access_to, 
			string("Modified %s's access level on channel %s to %i")).c_str(), 
		targetUser->getUserName().c_str(),
		theChan->getName().c_str(),
		newAccess);
	} // if( command == "ACCESS" )

if (command == "AUTOMODE")
	{
	/*
	 * Check we aren't trying to change someone with access higher
	 * than ours (or equal).
	 */
	
	if (level < targetLevel)
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
 
		// Only commit changes if this has been loaded from the Db.
		// (Ie: If its a forced temporary access, this flag
		// won't be set)..
		if (aLevel->getFlag(sqlLevel::F_ONDB))
			{
			aLevel->commit();
			}

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
 
		// Only commit changes if this has been loaded from the Db.
		// (Ie: If its a forced temporary access, this flag
		// won't be set)..
		if (aLevel->getFlag(sqlLevel::F_ONDB))
			{
			aLevel->commit();
			}

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
 
		// Only commit changes if this has been loaded from the Db.
		// (Ie: If its a forced temporary access, this flag won't
		// be set)..
		if (aLevel->getFlag(sqlLevel::F_ONDB))
			{
			aLevel->commit();
			}

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
