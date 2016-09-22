/**
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
 *             IE: /msg E kick #coder-com nick1,nick2,nick3 get outta 
 *		here!
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: KICKCommand.cc,v 1.11 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include        <string>

#include        "StringTokenizer.h"
#include        "ELog.h"
#include        "cservice.h"
#include        "Network.h"
#include        "levels.h"
#include        "responses.h"
#include	"match.h"

const char KICKCommand_cc_rcsId[] = "$Id: KICKCommand.cc,v 1.11 2003/06/28 01:21:20 dan_karrels Exp $" ;

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
	unsigned int takeMembersCount = (unsigned int)tmpChan->userList_size();

	if ((validUserMask(st[2])) && (level >= level::masskick))
	{
		if (validCIDRLength(st[2]))
		{
			/* Loop over all channel members, and match who to kick. */
			for(Channel::userIterator chanUsers = tmpChan->userList_begin(); chanUsers != tmpChan->userList_end(); ++chanUsers)
			{
				ChannelUser* tmpUser = chanUsers->second;

				if( (match(st[2], tmpUser->getClient()->getNickUserHost()) == 0) ||
					(match(st[2], tmpUser->getClient()->getRealNickUserHost()) == 0) )
				{
					/* Don't kick +k things */
					if (!tmpUser->getClient()->getMode(iClient::MODE_SERVICES))
						toBoot.push_back(tmpUser->getClient());
					else
						takeMembersCount--;
					/* Don't kick ourselves */
					if (tmpUser->getClient() != theClient)
						toBoot.push_back(tmpUser->getClient());
					else
						takeMembersCount--;
				}
			}
		}
		else	// <- not a valid cidr range
		{
			bot->Notice(theClient, "CIDR range for %s is too wide, maximum allowed is /32", st[2].c_str());
			return false;
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

	/*                                       */
	/* 	  ***   Take Over Protection   ***   */
	/*                                       */
	bool allmatched = false;
	if ((takeMembersCount > 1) && (takeMembersCount == (unsigned int)toBoot.size()))
		allmatched = true;

	if (level < 500)
	if (allmatched && validUserMask(st[2]) && (theChan->getFlag(sqlChannel::F_NOTAKE)))
	{
		string theMessage = TokenStringsParams("Take over attempt by %s (%s) on channel %s with kickmask %s",
				theClient->getNickName().c_str(), theUser->getUserName().c_str(), theChan->getName().c_str(), st[2].c_str());
		bot->NoteChannelManager(theChan, theMessage.c_str());
		//If revenge is Ignore then return
		if (theChan->getNoTake() == 1) return true;
		unsigned short banLevel = (unsigned short)level::set::notake;
		unsigned int banExpire = 7 * 86400;
		string banReason = "### Take Over Protection Triggered ###";
		string suspendReason = "\002*** TAKE OVER ATTEMPT ***\002";
		if (theChan->getNoTake() > 1)
			bot->doInternalBanAndKick(theChan, theClient, banLevel, banExpire, banReason);
		if (theChan->getNoTake() > 2)
			bot->doInternalSuspend(theChan, theClient, banLevel, banExpire, suspendReason);
		return true;
	}
	// *** End of Take Over Protection part **** //

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
