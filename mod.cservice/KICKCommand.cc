/*
* KICKCommand.cc
*
* 30/12/2000 - David Henriksen <david@itwebnet.dk>
* Initial Version.
*
* KICK one or more users from a channel, with access checking.
*
* 2001-03-16: Perry Lorier <isomer@coders.net>
* doesn't kick the initiator in a masskick
*
* Suggestion: Support several nicks by seperating them with a comma.
*             IE: /msg E kick #coder-com nick1,nick2,nick3 get outta here!
* $Id: KICKCommand.cc,v 1.10 2002/10/16 00:13:40 gte Exp $
*/

#include        <string>

#include        "StringTokenizer.h"
#include        "ELog.h"
#include        "cservice.h"
#include        "Network.h"
#include        "levels.h"
#include        "responses.h"
#include		"match.h"

const char KICKCommand_cc_rcsId[] = "$Id: KICKCommand.cc,v 1.10 2002/10/16 00:13:40 gte Exp $" ;

namespace gnuworld
{
using namespace gnuworld;
using namespace level;

bool KICKCommand::Exec( iClient* theClient, const string& Message )
{
	bot->incStat("COMMANDS.KICK");

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
	if (!theUser) return false;

	/*
	 *  Check the channel is actually registered.
	 */

	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
		    st[1].c_str());
		return false;
	}

 	/* Check the bot is in the channel. */

	if (!theChan->getInChan()) {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::i_am_not_on_chan,
				string("I'm not in that channel!")));
		return false;
	}

	/*
	 *  Check the user has sufficient access on this channel.
	 */

	int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
	if (level < level::kick)
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

	/*
	 *  Wildcard or normal kick?
	 */

	vector <iClient*> toBoot;

	if((bot->validUserMask(st[2])) && (level >= level::masskick))
	{
		/* Loop over all channel members, and match who to kick. */

		for(Channel::userIterator chanUsers = tmpChan->userList_begin(); chanUsers != tmpChan->userList_end(); ++chanUsers)
		{
			ChannelUser* tmpUser = chanUsers->second;

			if( (match(st[2], tmpUser->getClient()->getNickUserHost()) == 0) ||
			    (match(st[2], tmpUser->getClient()->getRealNickUserHost()) == 0) )
			{
				/* Don't kick +k things */
				if ( !tmpUser->getClient()->getMode(iClient::MODE_SERVICES) && tmpUser->getClient() != theClient )
				{
					toBoot.push_back(tmpUser->getClient());
				}
			}
		}

	} else {
		/*
		 *  Do a lookup on nickname, and check they are in the channel.
		 */

		iClient* target = Network->findNick(st[2]);

		if(!target)
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::dont_see_them).c_str(),
			    st[2].c_str());
			return false;
		}

		/*
		 *  Check they are on the channel.
		 */

		ChannelUser* tmpChanUser = tmpChan->findUser(target) ;
		if (!tmpChanUser)
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::cant_find_on_chan).c_str(),
				target->getNickName().c_str(), theChan->getName().c_str());
			return false;
		}

		/* Don't kick +k things */
		if ( target->getMode(iClient::MODE_SERVICES) )
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::wouldnt_appreciate,
					string("I don't think %s would appreciate that.")).c_str(),
				target->getNickName().c_str());
			return false;
		}

		toBoot.push_back(target);
	}

	if (toBoot.size() == 0)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::no_match,
				string("No Match!")));
		return false;
	}

	string args;
	if (st.size() >= 4)
	{
		args = st.assemble(3);
	} else
	{
		args = "No reason supplied";
	}

	string reason = "(" + theUser->getUserName() + ") ";
	reason += args;

	bot->Kick(tmpChan, toBoot, reason);
	return true ;
}

} // namespace gnuworld.
