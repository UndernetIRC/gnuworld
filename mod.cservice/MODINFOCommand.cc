/* 
 * MODINFOCommand.cc 
 *
 * 27/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Modifies a user's 'Level' record in a particular channel.
 *
 * Caveats: None
 *
 * $Id: MODINFOCommand.cc,v 1.2 2001/01/02 07:55:12 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"

const char MODINFOCommand_cc_rcsId[] = "$Id: MODINFOCommand.cc,v 1.2 2001/01/02 07:55:12 gte Exp $" ;

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
	if ((command != "ACCESS") && (command != "AUTOOP")) 
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

	int level = bot->getAccessLevel(theUser, theChan);
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
	 *  Check we aren't trying to change someone with access higher than ours.
	 */

	if (level <= targetLevel)
	{
		bot->Notice(theClient, "Cannot modify a user with equal or higher access than your own.");
		return false;
	}  

	/*
	 *  Figure out what they're doing - ACCESS or AUTOOP.
	 */


	if (command == "ACCESS")
	{ 
		/*
		 *	Check we aren't trying to set someone's access higher than ours.
		 */ 

		int newAccess = atoi(st[4].c_str());
		if ((newAccess <= 0) || (newAccess > 500))
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
		} 

	}
 
	if (command == "AUTOOP")
	{
		/*
		 *  Check for "Yes" or "No" and act accordingly.
		 */
	}

	return true ;
} 

} // namespace gnuworld.
