/*
 * UNBANCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Unbans a user from a channel.
 *
 * Caveats: None.
 *
 * $Id: UNBANCommand.cc,v 1.7 2001/02/04 23:37:32 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"
#include	"match.h"

const char UNBANCommand_cc_rcsId[] = "$Id: UNBANCommand.cc,v 1.7 2001/02/04 23:37:32 gte Exp $" ;

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
 
	/* Is the channel registered? */
	
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
	string banTarget = "";

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

		banTarget = aNick->getNickUserHost();
	} else
	{
		banTarget = st[2];
	}
	
	/*
	 *  Loop over all bans, removing any that match our target
	 */

	int banCount = 0;
	unsigned short comparison = 0;

	while (ptr != banList->end())
	{
		sqlBan* theBan = *ptr;
		/* 
		 * If we're matching by a users full host, reverse the way we check
		 * banmask.
		 */

		if ( isNick )
		{
			comparison = match(theBan->getBanMask(), banTarget);
		} else 
		{
			comparison = match(banTarget, theBan->getBanMask());
		}

		if ( comparison == 0 )
		{ 
			/* Matches! remove this ban - if we can. */ 
			if (theBan->getLevel() > level)
			{
				bot->Notice(theClient, "You have insufficient access to remove the ban %s from %s's database",
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

		if ( isNick )
		{
			comparison = match((*cPtr), banTarget);
		} else 
		{
			comparison = match(banTarget, (*cPtr));
		}

		if ( comparison == 0)
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

	} // while()

	bot->Notice(theClient, "Removed %i bans that matched %s",
		banCount, banTarget.c_str());

	return true;

} 

} // namespace gnuworld.

