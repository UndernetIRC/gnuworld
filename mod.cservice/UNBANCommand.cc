/*
 * UNBANCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Bans a user on a channel, adds this ban to the internal banlist.
 *
 * Caveats: None.
 *
 * $Id: UNBANCommand.cc,v 1.3 2001/01/25 00:19:13 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"

const char UNBANCommand_cc_rcsId[] = "$Id: UNBANCommand.cc,v 1.3 2001/01/25 00:19:13 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool UNBANCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
 
	if( st.size() < 3 )
	{
	    Usage(theClient);
	    return true;
	}
 
	// Is the channel registered?
	
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if(!theChan)
	{
	    bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
	    return false;
	} 

 	/* Check the bot is in the channel. */
 
	if (!theChan->getInChan()) {
		bot->Notice(theClient, "I'm not in that channel!");
		return false;
	} 

	// Is the user authorised?
	 
	sqlUser* theUser = bot->isAuthed(theClient, true);
	if(!theUser) return false;

	// Check level.

	int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
	if(level < level::unban)
	{
	    bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
	    return false;
	}

	vector< sqlBan* >* banList = bot->getBanRecords(theChan); 
	vector< sqlBan* >::iterator ptr = banList->begin(); 

	while (ptr != banList->end())
	{
		sqlBan* theBan = *ptr;
		
		if(string_lower(st[2]) == string_lower(theBan->getBanMask()))
		{
			/* Do we have enough access? */
			if (theBan->getLevel() > level)
			{
				bot->Notice(theClient, "You have insufficient access to remove that ban.");
			}
			else 
			{ 
				banList->erase(ptr);
				theBan->deleteRecord();
				bot->Notice(theClient, "Removed ban %s from %s",
					theBan->getBanMask().c_str(), theChan->getName().c_str()); 

				strstream s;
				s << bot->getCharYYXXX() << " M " << theChan->getName() << " -b "
				<< theBan->getBanMask() << ends;
			
				bot->Write( s );
				delete[] s.str();

				delete(theBan);				
				return true;
			}
		}  
 
	++ptr; 
	}
 
	bot->Notice(theClient, "Unable to find the ban %s on %s",
		st[2].c_str(), theChan->getName().c_str());

	return true ;
} 

} // namespace gnuworld.

