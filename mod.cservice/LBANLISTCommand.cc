/*
 * LBANLISTCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Lists internal bot banlist for a channel.
 *
 * Caveats: None.
 *
 * $Id: LBANLISTCommand.cc,v 1.4 2001/01/31 19:53:25 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"match.h"
 
#define MAX_RESULTS 15

const char LBANLISTCommand_cc_rcsId[] = "$Id: LBANLISTCommand.cc,v 1.4 2001/01/31 19:53:25 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool LBANLISTCommand::Exec( iClient* theClient, const string& Message )
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
	// Is the user authorised?
	 
	sqlUser* theUser = bot->isAuthed(theClient, true);
	if(!theUser) return false;
 
	bool showAll = false; /* Show all results? */
	for( StringTokenizer::const_iterator ptr = st.begin() ; ptr != st.end() ;
	++ptr )
	{
	if (string_lower(*ptr) == "-all")
		{ 
		sqlUser* tmpUser = bot->isAuthed(theClient, false);
		if ((tmpUser) && (bot->getAdminAccessLevel(tmpUser))) showAll = true; 
		continue;
		}
	}

	vector < sqlBan* >* banList = bot->getBanRecords(theChan); 

	bot->Notice(theClient,"\002*** Ban List for channel %s ***\002",
		theChan->getName().c_str());

	int results = 0;
	int ban_expires = 0;
	int ban_expires_d = 0;
	int ban_expires_f = 0;

	for( vector< sqlBan* >::const_iterator ptr = banList->begin() ; ptr != banList->end() ; ++ptr )
	{
		sqlBan* theBan = (*ptr);

		/* If its expired.. just don't show it - it'll be removed next JOIN. */ 
		if (theBan->getExpires() >= bot->currentTime()) 
		{
			if (match(st[2], theBan->getBanMask()) == 0)
			{
				results++;
				ban_expires = theBan->getExpires();
				ban_expires_d = ban_expires - bot->currentTime();
				ban_expires_f = bot->currentTime() - ban_expires_d;

				bot->Notice(theClient, "%s %s Level: %i", 
					theChan->getName().c_str(), theBan->getBanMask().c_str(), theBan->getLevel());
				bot->Notice(theClient, "ADDED BY: %s (%s)",
					theBan->getSetBy().c_str(), theBan->getReason().c_str());
				bot->Notice(theClient, "SINCE: %i", theBan->getSetTS());
				bot->Notice(theClient, "EXP: %s", bot->prettyDuration(ban_expires_f).c_str());
			}
		}
		if ((results >= MAX_RESULTS) && !showAll) break;
	}

	if ((results >= MAX_RESULTS) && !showAll)
	{
		bot->Notice(theClient, "There are more than 15 matching entries.");
		bot->Notice(theClient, "Please restrict your query."); 
	} else if (results > 0)
	{
		bot->Notice(theClient,"\002*** END ***\002");
	} 
		else
	{
		bot->Notice(theClient, "No Match!");
	}


 
	return true ;
} 

} // namespace gnuworld.

