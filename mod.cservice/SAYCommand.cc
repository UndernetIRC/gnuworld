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
 * $Id: SAYCommand.cc,v 1.5 2001/03/21 10:22:04 isomer Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char SAYCommand_cc_rcsId[] = "$Id: SAYCommand.cc,v 1.5 2001/03/21 10:22:04 isomer Exp $" ;

namespace gnuworld
{

using std::string ;
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
if (!theUser)
	{
	return false; 
	}

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::say)
	{
/* CSC get far too many questions as to how to use these commands
 * so they prefer it kept quiet about 'em.  Fair enough too.
 *
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
 */
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

bot->Message(st[1], st.assemble(2).c_str());
return true;
} 

} // namespace gnuworld.

