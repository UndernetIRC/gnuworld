/* 
 * PARTCommand.cc 
 *
 * 10/02/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version. Written, and finished.
 *
 * Allows an oper to part cmaster from a registered channel.
 *
 * Caveats: None
 *
 * $Id: OPERPARTCommand.cc,v 1.1 2001/02/10 23:34:02 gte Exp $
 */


#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

const char OPERPARTCommand_cc_rcsId[] = "$Id: OPERPARTCommand.cc,v 1.1 2001/02/10 23:34:02 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool OPERPARTCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	/* 
	 *  Check the channel is actually registered.
	 */

	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, "The channel %s doesn't appear to be registered",
			st[1].c_str());
		return false;
	} 

	/*
	 *  Check if the user is an oper.
	 */

    if(!theClient->isOper())
            {
            bot->Notice(theClient, "This command is reserved to IRC Operators");
            return true;
            }

	/* Check the bot is in the channel. */
 
	if (!theChan->getInChan()) {
		bot->Notice(theClient, "I'm not in that channel!");
		return false;
	}
 
    strstream s;

    s       << server->getCharYY() << " WA :"
            << theClient->getNickUserHost() << " is asking me to leave "
            << theChan->getName() << ends;
    bot->Write(s);
    delete[] s.str();
	 
	theChan->setInChan(false);
	bot->getUplink()->UnRegisterChannelEvent(theChan->getName(), bot);
	
	bot->Part(theChan->getName());
	
	return true;
} 

} // namespace gnuworld.
