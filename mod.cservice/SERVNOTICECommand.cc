/* 
 * SERVNOTICECommand.cc 
 *
 * 03/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Sends a server notice to a channel.
 *
 * Caveats: None
 *
 * $Id: SERVNOTICECommand.cc,v 1.2 2001/03/07 15:10:53 dan_karrels Exp $
 */

#include	<string>
#include	<map>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char SERVNOTICECommand_cc_rcsId[] = "$Id: SERVNOTICECommand.cc,v 1.2 2001/03/07 15:10:53 dan_karrels Exp $" ;

namespace gnuworld
{

using std::map ;

bool SERVNOTICECommand::Exec( iClient* theClient, const string& Message )
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

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false;
	}
 
/*
 *  Check the user has sufficient admin access to do this.
 */

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::servnotice)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::insuf_access).c_str());
	return false;
	} 

Channel* tmpChan = Network->findChannel(st[1]); 
if (!tmpChan) 
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_is_empty).c_str(), 
		st[1].c_str());
	return false;
	} 

string theMessage = st.assemble(2);
bot->serverNotice(tmpChan, theMessage.c_str());

return true ;
} 

} // namespace gnuworld.

