/* 
 * INVITECommand.cc 
 *
 * 29/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Invites a user to a channel they have access on.
 *
 * Caveats: None
 *
 * $Id: INVITECommand.cc,v 1.1 2000/12/30 05:47:29 gte Exp $
 */


#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

const char INVITECommand_cc_rcsId[] = "$Id: INVITECommand.cc,v 1.1 2000/12/30 05:47:29 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool INVITECommand::Exec( iClient* theClient, const string& Message )
{ 
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
	if (level < level::invite)
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str());
		return false;
	} 

	if( st.size() < 3 )
	{
		/*
		 *  No parameters, Just invite them to the channel.
		 */

		bot->Invite(theClient, theChan->getName());
		return true;
	}

 	/*
	 *  They've supplied a nick, invite them instead.
	 */

	iClient* target = Network->findNick(st[2]);
	if(!target)
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::dont_see_them).c_str(),
			st[2].c_str()); 
		return false;
	}
						
	bot->Invite(target, theChan->getName()); 

	if(target != theClient) // Don't send a notice to the person who issued the command. 
	{ 
		sqlUser* tmpTargetUser = bot->isAuthed(target, false);
		if (tmpTargetUser)
		{
			bot->Notice(target, bot->getResponse(tmpTargetUser, language::invited_by).c_str(),
				theUser->getUserName().c_str(), theChan->getName().c_str());
		} else 
		{
			bot->Notice(target, bot->getResponse(theUser, language::invited_by).c_str(),
				theUser->getUserName().c_str(), theChan->getName().c_str());
		} 
	}

	return true ;
} 

} // namespace gnuworld.
