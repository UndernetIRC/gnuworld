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
 * $Id: UNBANCommand.cc,v 1.1 2001/01/13 20:51:33 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"

const char UNBANCommand_cc_rcsId[] = "$Id: UNBANCommand.cc,v 1.1 2001/01/13 20:51:33 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool UNBANCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
 
	if( st.size() < 4 )
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

	// Check level.

	int level = bot->getAccessLevel(theUser, theChan);
	if(level < level::ban)
	{
	    bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
	    return false;
	}

	return true ;
} 

} // namespace gnuworld.

