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
 * $Id: OPERPARTCommand.cc,v 1.9 2001/05/11 16:39:13 gte Exp $
 */


#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

const char OPERPARTCommand_cc_rcsId[] = "$Id: OPERPARTCommand.cc,v 1.9 2001/05/11 16:39:13 gte Exp $" ;

namespace gnuworld
{

using std::string ;
 
bool OPERPARTCommand::Exec( iClient* theClient, const string& Message )
{ 
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

/*
 *  Check if the user is an oper.
 */

// cservice::getResponse() will properly handle a NULL
// sqlUser
sqlUser* theUser = bot->isAuthed(theClient, false);

if(!theClient->isOper())
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::ircops_only_cmd,
		"This command is reserved to IRC Operators"));
	return true;
	}

/* 
 *  Check the channel is actually registered.
 */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_not_reg).c_str(),
		st[1].c_str());
	return false;
	} 

/* Check the bot is in the channel. */

if (!theChan->getInChan())
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser, language::i_am_not_on_chan,
			string("I'm not in that channel!")));
	return false;
	}

bot->writeChannelLog(theChan, theClient, sqlChannel::EV_OPERPART, "");

// Tell the world. 
 
strstream s;
s       << server->getCharYY()
	<< " WA :"
	<< "An IRC Operator is asking me to leave channel "
	<< theChan->getName()
	<< ends;

bot->Write(s);
delete[] s.str();
	
bot->logAdminMessage("%s is asking me to leave channel %s",
		theClient->getNickUserHost().c_str(),
		theChan->getName().c_str());

theChan->setInChan(false);
bot->getUplink()->UnRegisterChannelEvent(theChan->getName(), bot);
bot->joinCount--;
	
bot->Part(theChan->getName(), "At the request of an IRC Operator");
	
return true;
} 

} // namespace gnuworld.
