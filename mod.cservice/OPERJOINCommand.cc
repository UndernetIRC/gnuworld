/* 
 * OPERJOINCommand.cc 
 *
 * 10/02/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version. Written, and finished.
 *
 * Allows an oper to make cmaster join a registered channel.
 *
 * Caveats: None
 *
 * $Id: OPERJOINCommand.cc,v 1.5 2001/02/15 23:31:33 gte Exp $
 */


#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

const char OPERJOINCommand_cc_rcsId[] = "$Id: OPERJOINCommand.cc,v 1.5 2001/02/15 23:31:33 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool OPERJOINCommand::Exec( iClient* theClient, const string& Message )
{ 
	/*
	 *  Check the user is an oper.
	 */

	if(!theClient->isOper())
	{
		bot->Notice(theClient, "This command is reserved to IRC Operators");
		return true;
	}

	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	/* 
	 *  Check the channel is actually registered.
	 */

	Channel* tmpChan = Network->findChannel(st[1]);
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, "The channel %s doesn't appear to be registered",
			st[1].c_str());
		return false;
	} 

 	/* Check the bot isn't in the channel. */ 
	if (theChan->getInChan()) {
		bot->Notice(theClient, "I'm already in that channel!");
		return false;
	}
 
        // Tell the world.
 
	strstream s;
	
	s	<< server->getCharYY() << " WA :"
		<< "An IRC Operator is asking me to join channel "
		<< theChan->getName() << ends;
	bot->Write(s);
	delete[] s.str();

	bot->logAdminMessage("%s is asking me to join channel %s",
			theClient->getNickUserHost().c_str(),
			theChan->getName().c_str());

	bot->writeChannelLog(theChan, theClient, sqlChannel::EV_OPERJOIN, "");

	theChan->setInChan(true);
	bot->getUplink()->RegisterChannelEvent( theChan->getName(), bot);
	bot->Join(theChan->getName(), theChan->getChannelMode(), theChan->getChannelTS(), false);

	/* Whack this reop on the Q */ 
	bot->reopQ.insert(cservice::reopQType::value_type(theChan->getName(), bot->currentTime() + 15) );
 
	if (tmpChan)
		{
		if(theChan->getFlag(sqlChannel::F_NOOP)) bot->deopAllOnChan(tmpChan);
		if(theChan->getFlag(sqlChannel::F_STRICTOP)) bot->deopAllUnAuthedOnChan(tmpChan);
		}
		
	
	return true;
} 

} // namespace gnuworld.
