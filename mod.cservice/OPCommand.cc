/* OPCommand.cc */ 

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"

const char OPCommand_cc_rcsId[] = "$Id: OPCommand.cc,v 1.6 2000/12/24 17:05:07 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool OPCommand::Exec( iClient* theClient, const string& Message )
{ 
	vector< iClient* > opList; // List of clients to op. 
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
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
		return false;
	} 

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) {
		return false;
	}
 
	/*
	 *  Check the user has sufficient access on this channel.
	 */

	int level = bot->getAccessLevel(theUser, theChan);
	if (level < level::op)
	{
		bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
		return false;
	} 

	Channel* tmpChan = Network->findChannel(theChan->getName()); 
	if (!tmpChan) 
	{
		bot->Notice(theClient, "Sorry, %s is empty.", theChan->getName().c_str());
		return false;
	}
 

	if( st.size() < 3 ) // No nicks provided, assume we op ourself. :)
	{
		opList.push_back(theClient);
	}

	/*
	 *  Loop over the remaining 'nick' parameters, opping them all.
	 */

	iClient* target;
	unsigned short counter = 2; // Offset of first nick in list.
	unsigned short cont = true;

	while (counter < st.size())
	{ 
		target = Network->findNick(st[counter]);

		if(!target)
		{
			bot->Notice(theClient, "I don't see %s anywhere.", st[counter].c_str());
			cont = false;
		} else { 
			ChannelUser* tmpChanUser = tmpChan->findUser(target) ; 
			if (!tmpChanUser) 
			{
				bot->Notice(theClient, "Sorry, %s isn't on %s.", target->getNickName().c_str(), theChan->getName().c_str()); 
				cont = false;
			}
		}
		
	 	if (cont) opList.push_back(target);
		cont = true;
		counter++;
	}

	// Op them. 
	bot->Op(tmpChan, opList);
	return true ;
} 

} // namespace gnuworld.

