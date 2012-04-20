/**
 * INVITECommand.cc
 *
 * 29/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Invites a user to a channel they have access on.
 *
 * Caveats: None
 *
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
 * $Id: INVITECommand.cc,v 1.7 2003/06/28 01:21:20 dan_karrels Exp $
 */


#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

const char INVITECommand_cc_rcsId[] = "$Id: INVITECommand.cc,v 1.7 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{

using namespace gnuworld;

bool INVITECommand::Exec( iClient* theClient, const string& Message )
{
	bot->incStat("COMMANDS.INVITE");
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

	char delim = 0;
	string source;
	int i = 1;
	string::size_type pos = st[1].find_first_of(',');
	/* Found a comma? */
	if (string::npos != pos) {
		/* We'll do a comma seperated search then. */
		source = st.assemble(1);
		delim = ',';
		i = 0;
	} else {
		source = Message;
		delim = ' ';
	}
	StringTokenizer st2(source, delim);
	int max_channels = st2.size() < 10 ? st2.size() : 10;
	for(; i < max_channels;++i) {
		sqlChannel* theChan = bot->getChannelRecord(st2[i]);
		if (!theChan) {
			bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
				st2[i].c_str());
			continue;
		}

		/* Check the bot is in the channel. */

		if (!theChan->getInChan()) {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::i_am_not_on_chan,
					string("I'm not in that channel!")));
			continue;
		}

		/*
		 *  Check the user has sufficient access on this channel.
		 */

		int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
		if (level < level::invite)
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str());
			continue;
		}

		sqlBan* tmpBan = bot->isBannedOnChan(theChan, theClient);
		if (tmpBan && tmpBan->getLevel() >= 75) {
			bot->Notice(theClient,"Can't invite you to channel %s, you are banned",theChan->getName().c_str());
			continue;
		}
		
		/*
		 *  No parameters, Just invite them to the channel.
		 */

		bot->Invite(theClient, theChan->getName());
	}
	return true;
}

} // namespace gnuworld.
