/* 
 * OPCommand.cc 
 *
 * 20/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 28/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Added multilingual support.
 *
 * OP's one or more users on a channel the user as access on.
 *
 * Caveats: None
 *
 * $Id: OPCommand.cc,v 1.7 2000/12/28 21:19:53 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char OPCommand_cc_rcsId[] = "$Id: OPCommand.cc,v 1.7 2000/12/28 21:19:53 gte Exp $" ;

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
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) {
		return false;
	}

	/* 
	 *  Check the channel is actually registered.
	 */

	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
			st[1].c_str());
		return false;
	} 

	/*
	 *  Check the user has sufficient access on this channel.
	 */

	int level = bot->getAccessLevel(theUser, theChan);
	if (level < level::op)
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str());
		return false;
	} 

	Channel* tmpChan = Network->findChannel(theChan->getName()); 
	if (!tmpChan) 
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_is_empty).c_str(), 
			theChan->getName().c_str());
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
			bot->Notice(theClient, bot->getResponse(theUser, language::dont_see_them).c_str(),
				st[counter].c_str());
			cont = false;
		} else 
		{ 
			ChannelUser* tmpChanUser = tmpChan->findUser(target) ; 
			if (!tmpChanUser) 
			{
				bot->Notice(theClient, bot->getResponse(theUser, language::cant_find_on_chan).c_str(), 
					target->getNickName().c_str(), theChan->getName().c_str()); 
				cont = false;
			}
		}
		
	 	if (cont) 
	 	{
			opList.push_back(target); 
			if(target != theClient) // Don't send a notice to the person who issued the command.
			{ 
				sqlUser* tmpTargetUser = bot->isAuthed(target, false);
				if (tmpTargetUser)
				{
					bot->Notice(target, bot->getResponse(tmpTargetUser, language::youre_opped_by).c_str(),
						theUser->getUserName().c_str());
				} else 
				{
					bot->Notice(target, bot->getResponse(theUser, language::youre_opped_by).c_str(),
						theUser->getUserName().c_str());
				} 
			}
		}

		cont = true;
		counter++;
	}

	// Op them. 
	bot->Op(tmpChan, opList);
	return true ;
} 

} // namespace gnuworld.

