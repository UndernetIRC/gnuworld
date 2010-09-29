/**
 * DEVOICECommand.cc
 *
 * 28/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Devoice's one or more users on a channel the user as access on.
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
 * $Id: DEVOICECommand.cc,v 1.10 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>
#include	<map>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

using std::map ;

const char DEVOICECommand_cc_rcsId[] = "$Id: DEVOICECommand.cc,v 1.10 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{

using namespace gnuworld;

bool DEVOICECommand::Exec( iClient* theClient, const string& Message )
{
	bot->incStat("COMMANDS.DEVOICE");

	vector< iClient* > devoiceList; // List of clients to devoice.
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
	if (level < level::devoice)
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


	if( st.size() < 3 ) /* No nicks provided, assume we devoice ourself. :) */
	{
		devoiceList.push_back(theClient);
	}

	/*
	 *  Loop over the remaining 'nick' parameters, opping them all.
	 */

	iClient* target;
	unsigned short counter = 2; // Offset of first nick in list.
	unsigned short cont = true;
	typedef map < iClient*, int > duplicateMapType;
	duplicateMapType duplicateMap;
	string source;
	char delim;

	if( st.size() < 3 ) // No nicks provided, assume we op ourself. :)
	{
		devoiceList.push_back(theClient);
		source = Message;
		delim = ' ';
	} else
	{
		string::size_type pos = st[2].find_first_of( ',' ) ;
		if( string::npos != pos ) // Found a comma?
		{
			source = st.assemble(2); // We'll do a comma seperated search then.
			delim = ',';
			counter = 0;
		} else {
			source = Message;
			delim = ' ';
		}
	}

	StringTokenizer st2( source, delim );

	while (counter < st2.size())
	{
		target = Network->findNick(st2[counter]);

		if(!target)
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::dont_see_them).c_str(),
				st2[counter].c_str());
			cont = false;
		}

		ChannelUser* tmpChanUser;
		if (cont) tmpChanUser = tmpChan->findUser(target) ;
		if (cont && !tmpChanUser) // User isn't on the channel?
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::cant_find_on_chan).c_str(),
				target->getNickName().c_str(), theChan->getName().c_str());
			cont = false;
		}

		if(cont && !tmpChanUser->getMode(ChannelUser::MODE_V)) // User isn't voiced?
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::not_voiced).c_str(),
				target->getNickName().c_str(), theChan->getName().c_str());
				cont = false;
		}

	 	if (cont)
	 	{
			duplicateMapType::iterator ptr = duplicateMap.find(target); // Check for duplicates.
			if(ptr == duplicateMap.end()) // Not a duplicate.
			{
				devoiceList.push_back(target);
				duplicateMap.insert(duplicateMapType::value_type(target, 0));

				if(target != theClient) // Don't send a notice to the person who issued the command.
				{
					sqlUser* tmpTargetUser = bot->isAuthed(target, false);
					if (tmpTargetUser)
					{
						bot->Notice(target, bot->getResponse(tmpTargetUser, language::youre_devoiced_by).c_str(),
							theClient->getNickName().c_str(),
							theUser->getUserName().c_str(),
							theChan->getName().c_str());
					} else
					{
						bot->Notice(target, bot->getResponse(theUser, language::youre_devoiced_by).c_str(),
							theClient->getNickName().c_str(),
							theUser->getUserName().c_str(),
							theChan->getName().c_str());
					}
				} // Don't send to person who issued.
		   	} // Not a duplicate.
		}

		cont = true;
		counter++;
	}

	// devoice them.
	bot->DeVoice(tmpChan, devoiceList);
	return true ;
}

} // namespace gnuworld.

