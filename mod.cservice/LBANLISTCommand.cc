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
 * $Id: LBANLISTCommand.cc,v 1.1 2001/01/13 20:51:33 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"

const char LBANLISTCommand_cc_rcsId[] = "$Id: LBANLISTCommand.cc,v 1.1 2001/01/13 20:51:33 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool LBANLISTCommand::Exec( iClient* theClient, const string& Message )
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

	return true ;
} 

} // namespace gnuworld.

