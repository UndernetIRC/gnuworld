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
 * $Id: UNBANCommand.cc,v 1.6 2001/02/02 20:17:07 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"
#include	"match.h"

const char UNBANCommand_cc_rcsId[] = "$Id: UNBANCommand.cc,v 1.6 2001/02/02 20:17:07 gte Exp $" ;

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

	Channel* theChannel = Network->findChannel(theChan->getName()); 
	if (!theChannel) 
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_is_empty).c_str(), 
		theChan->getName().c_str());
		return false;
	} 
 
	vector< sqlBan* >* banList = bot->getBanRecords(theChan); 
	vector< sqlBan* >::iterator ptr = banList->begin(); 

 	/*
	 *  Are they trying to unban by nick or hostmask?
	 */

	bool isNick = bot->validUserMask( st[2] ) ? false : true ;

	/* Try by nickname first, remove any bans that match this users host */
	
	if( isNick )
	{
		iClient* aNick = Network->findNick(st[2]);
		if(!aNick)
		{
			bot->Notice(theClient, "Sorry, I cannot find the specified nick.");
			return true;
		}
	 
		/*
		 *  Loop over all bans, removing any that match this users current
		 *  n!u@host.
		 */

		int banCount = 0;
		while (ptr != banList->end())
		{
			sqlBan* theBan = *ptr;
			if ( match(theBan->getBanMask(), aNick->getNickUserHost()) == 0 )
			{ 
				/* Matches! remove this ban - if we can. */ 
				if (theBan->getLevel() >= level)
				{
					bot->Notice(theClient, "You have insufficient access to remove the ban %s from %s",
						theBan->getBanMask().c_str(), theChan->getName().c_str());
					++ptr;
				}
				else 
				{ 
					bot->UnBan(theChannel, theBan->getBanMask());
					ptr = banList->erase(ptr);
					theBan->deleteRecord();
					delete(theBan);
					banCount++;
				}
			} // if (banMatched)
				else
			{
				++ptr;
			} 

		} // while()


		/*
		 *  Scan through the channel banlist too, and attempt to match any.
		 */

		Channel::const_banIterator cPtr = theChannel->banList_begin();
		while (cPtr != theChannel->banList_end())
		{
			if ( match((*cPtr), aNick->getNickUserHost()) == 0)
			{ 
	 			// Can't call xClient::UnBan inside the loop it will modify without
				// a return value.
				strstream s;
				s	<< bot->getCharYYXXX() << " M " << theChannel->getName()
					<< " -b " << (*cPtr) << ends; 
				bot->Write( s );
				delete[] s.str();

				theChannel->removeBan(*cPtr);
				cPtr = theChannel->banList_begin();

				banCount++;
			} else
			{
				++cPtr;
			}


		}
 
		bot->Notice(theClient, "Removed %i bans that matched %s",
			banCount, aNick->getNickUserHost().c_str());
		return true;
	} // If (isNick)

	
	/* Otherwise, try to delete by supplied mask */ 
 
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

				bot->UnBan(theChannel, theBan->getBanMask());
 
				delete(theBan); 
				return true;
			}
		}  
 
	++ptr; 
	}

	/*
	 *  If we get here, we've not found it in the channel banlist.
	 *  Attempt to just remove the ban from IRC anyway..
	 */

	bot->UnBan(theChannel, st[2]); 
 
	return true ;
} 

} // namespace gnuworld.

