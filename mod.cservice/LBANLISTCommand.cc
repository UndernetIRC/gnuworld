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
 * $Id: LBANLISTCommand.cc,v 1.2 2001/01/24 01:13:51 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"

const char LBANLISTCommand_cc_rcsId[] = "$Id: LBANLISTCommand.cc,v 1.2 2001/01/24 01:13:51 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool LBANLISTCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
 
	if( st.size() < 2 )
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

	vector < sqlBan* >* banList = bot->getBanRecords(theChan); 

	bot->Notice(theClient,"\002*** Ban List for channel %s ***\002",
		theChan->getName().c_str());

	for( vector< sqlBan* >::const_iterator ptr = banList->begin() ; ptr != banList->end() ; ++ptr )
	{
		sqlBan* theBan = (*ptr);
		bot->Notice(theClient, "%s %s Level: %i", 
			theChan->getName().c_str(), theBan->getBanMask().c_str(), theBan->getLevel());
		bot->Notice(theClient, "ADDED BY: %s (%s)",
			theBan->getSetBy().c_str(), theBan->getReason().c_str());
		bot->Notice(theClient, "SINCE: %i", theBan->getSetTS());
		bot->Notice(theClient, "EXP: %i", theBan->getExpires());
	}
	bot->Notice(theClient,"\002*** END ***\002");
 
	return true ;
} 

} // namespace gnuworld.

