/* UNFORCECommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"

const char UNFORCECommand_cc_rcsId[] = "$Id: UNFORCECommand.cc,v 1.6 2001/03/13 22:39:33 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool UNFORCECommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false; 
 
	sqlChannel* admChan = bot->getChannelRecord("*");

	int admLevel = bot->getAccessLevel(theUser, admChan);
	if (admLevel < level::force)
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::insuf_access,
				string("Sorry, you have insufficient access to perform that command.")));
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
 	 * Check to see if this userID is forced in this channel.
	 */

	sqlChannel::forceMapType::iterator ptr = theChan->forceMap.find(theUser->getID());
	/* If we found something, drop it. */
	if(ptr != theChan->forceMap.end())
		{ 
		theChan->forceMap.erase(theUser->getID());
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::rem_temp_access,
					string("Removed your temporary access of %i from channel %s")).c_str(), 
				admLevel, theChan->getName().c_str()); 
			return true;
		} 

	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::no_forced_access,
			string("You don't appear to have a forced access in %s, perhaps it expired?")).c_str(), 
		theChan->getName().c_str()); 
	return true ;
} 

} // namespace gnuworld.

