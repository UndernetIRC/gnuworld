/*
 * SAYCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Command to keep Kev happy :)
 *
 * Caveats: None.
 *
 * $Id: SAYCommand.cc,v 1.2 2001/01/31 19:53:25 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"

const char SAYCommand_cc_rcsId[] = "$Id: SAYCommand.cc,v 1.2 2001/01/31 19:53:25 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool SAYCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
 
	if( st.size() < 3 )
	{
	    Usage(theClient);
	    return true;
	}

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, false);
	if (!theUser) return false; 
 
	int admLevel = bot->getAdminAccessLevel(theUser);
	if (admLevel < level::say)
	{
		bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
		return false;
	} 

 	/*
	 *  First, check the channel is registered.
	 */
 
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) 
	{
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
		return false;
	} 
 
	bot->Message(st[1], st.assemble(2).c_str());
 	return true;
} 

} // namespace gnuworld.

