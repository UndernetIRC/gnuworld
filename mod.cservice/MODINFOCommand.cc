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
 * $Id: MODINFOCommand.cc,v 1.10 2001/01/30 03:02:46 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"

const char MODINFOCommand_cc_rcsId[] = "$Id: MODINFOCommand.cc,v 1.10 2001/01/30 03:02:46 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
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
	 *  First, check the channel is registered.
	 */
 
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
		return false;
	} 

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false; 
 
	/*
	 *  Check the user has sufficient access on this channel. 
	 */ 
	int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
	if (level < level::modinfo)
	{
		bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
		return false;
	} 

	/*
	 *  Check the person we're trying to change actually exists.
	 */

	sqlUser* targetUser = bot->getUserRecord(st[3]);
	if (!targetUser)
	{
		bot->Notice(theClient, "Sorry, I don't know who %s is.", st[3].c_str());
		return false; 
	}
 
	/*
	 *  Check this user really does have access on this channel.
	 */
 
	int targetLevel = bot->getAccessLevel(targetUser, theChan);
	if (targetLevel == 0)
	{
		bot->Notice(theClient, "%s doesn't appear to have access in %s.", targetUser->getUserName().c_str(), theChan->getName().c_str());
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
		 *  Check we aren't trying to change someone with access higher (or equal) than ours.
		 */
	
		if (level <= targetLevel)
		{
			/* If the access is forced, they are allowed to modify their own record. */
			if (!tmpLevel->getFlag(sqlLevel::F_FORCED))
			{ 
				bot->Notice(theClient, "Cannot modify a user with equal or higher access than your own.");
				return false; 
			}
		}
	
		/*
		 *	Check we aren't trying to set someone's access higher than ours.
		 */ 

		int newAccess = atoi(st[4].c_str());
		if ((newAccess <= 0) || (newAccess > 999))
		{
			bot->Notice(theClient, "Invalid access level.");
			return false;
		}

		/*
		 *  And finally, check they aren't trying to give someone higher access than them.
		 */

		if (level <= newAccess)
		{
			bot->Notice(theClient, "Cannot give a user higher or equal access to your own.");
			return false;
		} 
 
		sqlLevel* aLevel = bot->getLevelRecord(targetUser, theChan);
		aLevel->setAccess(newAccess);
		aLevel->setLastModif(bot->currentTime());
		aLevel->setLastModifBy(theClient->getNickUserHost());

		// Only commit changes if this has been loaded from the Db.
		// (Ie: If its a forced temporary access, this flag won't be set)..
		if (aLevel->getFlag(sqlLevel::F_ONDB)) aLevel->commit();
		bot->Notice(theClient, "Modified %s's access level on channel %s to %i", 
			targetUser->getUserName().c_str(), theChan->getName().c_str(),
			newAccess);
	}
 

	if (command == "AUTOMODE")
	{
		
		/*
		 *  Check we aren't trying to change someone with access higher than ours (or equal).
		 */
	
		if (level < targetLevel)
		{
			bot->Notice(theClient, "Cannot modify a user with higher access than your own.");
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
			aLevel->setLastModif(bot->currentTime());
			aLevel->setLastModifBy(theClient->getNickUserHost());
 
			// Only commit changes if this has been loaded from the Db.
			// (Ie: If its a forced temporary access, this flag won't be set)..
			if (aLevel->getFlag(sqlLevel::F_ONDB)) aLevel->commit();

			bot->Notice(theClient, "Set AUTOMODE to OP for %s on channel %s", 
				targetUser->getUserName().c_str(), theChan->getName().c_str()); 
			return false;
		}

		if (string_upper(st[4]) == "VOICE")
		{
			sqlLevel* aLevel = bot->getLevelRecord(targetUser, theChan);
			aLevel->removeFlag(sqlLevel::F_AUTOOP);
			aLevel->setFlag(sqlLevel::F_AUTOVOICE); 
			aLevel->setLastModif(bot->currentTime());
			aLevel->setLastModifBy(theClient->getNickUserHost());
 
			// Only commit changes if this has been loaded from the Db.
			// (Ie: If its a forced temporary access, this flag won't be set)..
			if (aLevel->getFlag(sqlLevel::F_ONDB)) aLevel->commit();

			bot->Notice(theClient, "Set AUTOMODE to VOICE for %s on channel %s", 
				targetUser->getUserName().c_str(), theChan->getName().c_str()); 
			return false;
		} 

		if (string_upper(st[4]) == "NONE")
		{
			sqlLevel* aLevel = bot->getLevelRecord(targetUser, theChan);			
			aLevel->removeFlag(sqlLevel::F_AUTOOP);
			aLevel->removeFlag(sqlLevel::F_AUTOVOICE);
			aLevel->setLastModif(bot->currentTime());
			aLevel->setLastModifBy(theClient->getNickUserHost());
 
			// Only commit changes if this has been loaded from the Db.
			// (Ie: If its a forced temporary access, this flag won't be set)..
			if (aLevel->getFlag(sqlLevel::F_ONDB)) aLevel->commit();

			bot->Notice(theClient, "Set AUTOMODE to NONE for %s on channel %s", 
				targetUser->getUserName().c_str(), theChan->getName().c_str()); 
			return false;
		}

		Usage(theClient);
		return true; 
	}

	return true ;
} 

} // namespace gnuworld.
