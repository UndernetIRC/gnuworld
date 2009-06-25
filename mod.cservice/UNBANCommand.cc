/**
 * UNBANCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Unbans a user from a channel.
 *
 * Caveats: None.
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
 * $Id: UNBANCommand.cc,v 1.22 2009/06/25 19:05:23 mrbean_ Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"
#include	"match.h"
#include	"cidr.h"

const char UNBANCommand_cc_rcsId[] = "$Id: UNBANCommand.cc,v 1.22 2009/06/25 19:05:23 mrbean_ Exp $" ;

namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

using namespace level;

bool UNBANCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.UNBAN");

StringTokenizer st( Message ) ;

if( st.size() < 3 )
	{
    Usage(theClient);
    return true;
	}

// Is the user authorised?

sqlUser* theUser = bot->isAuthed(theClient, true);
if(!theUser)
	{
	return false;
	}

/* Is the channel registered? */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if(!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
		language::chan_not_reg,
		string("Sorry, %s isn't registered with me.")).c_str(),
	st[1].c_str());

	return false;
	}

/* Check the bot is in the channel. */

if (!theChan->getInChan())
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::i_am_not_on_chan,
			string("I'm not in that channel!")));
	return false;
	}

Channel* theChannel = Network->findChannel(theChan->getName());
if (!theChannel)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_is_empty).c_str(),
		theChan->getName().c_str());
	return false;
	}

/*
 * Check we're actually opped first..
 */

ChannelUser* tmpBotUser = theChannel->findUser(bot->getInstance());
if (!tmpBotUser) return false;
if(!tmpBotUser->getMode(ChannelUser::MODE_O))
		{
		bot->Notice(theClient, bot->getResponse(theUser,
			language::im_not_opped, "I'm not opped in %s").c_str(),
			theChan->getName().c_str());
		return false;
		}

// Check level.

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if(level < level::unban)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to "
				"perform that command.")));
	return false;
	}

/*
 *  Are they trying to unban by nick or hostmask?
 */
bool isNick = bot->validUserMask( st[2] ) ? false : true ;

/* Try by nickname first, remove any bans that match this users host */
string banTarget ;
bool isCIDR = false;
if( isNick )
	{
	iClient* aNick = Network->findNick(st[2]);
	if(!aNick)
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::cant_find_on_chan,
				string("I can't find %s on channel %s")).c_str(),
				st[2].c_str(),
				theChan->getName().c_str());
		return true;
		}

	banTarget = aNick->getNickUserHost();
	}
else
	{
	banTarget = st[2];
	isCIDR = xCIDR(banTarget).GetValid();
	}

/*
 *  Loop over all bans, removing any that match our target
 */
std::map< int,sqlBan* >::iterator ptr = theChan->banList.begin();

size_t banCount = 0;
unsigned short comparison = 0;
unsigned short exactmatch = 0;
vector <sqlBan*> oldBans;

while (ptr != theChan->banList.end())
	{
	sqlBan* theBan = ptr->second;
	/*
	 * If we're matching by a users full host, reverse the way we check
	 * banmask.
	 */

	/* do a (case insensitive) literal match */
	if (!strcasecmp(theBan->getBanMask(), banTarget))
		exactmatch = 1;

	if ( isNick )
		{
		comparison = match(theBan->getBanMask(), banTarget);
		}
	else if(!isCIDR)
		{
		comparison = match(banTarget, theBan->getBanMask());
		}

	else 
		{
		comparison = 1; //Its a cidr, No match
		}
	if ( comparison == 0 )
		{
		/* Matches! remove this ban - if we can. */
		if (theBan->getLevel() > level)
			{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::cant_rem_ban,
					string("You have insufficient access to remove the ban %s from %s's database")).c_str(),
				theBan->getBanMask().c_str(),
				theChan->getName().c_str());
			}
		else
			{
			/* if it's an exact match, we want to only remove that ban */
			if (exactmatch == 1)
			{
				oldBans.clear();
				oldBans.push_back(theBan);
				break;
			} else {
				oldBans.push_back(theBan);
			}
			}
		} // if (banMatched)
	++ptr;

	} // while()
/*
 *  Remove all the matching bans
 */
 
vector<sqlBan*>::iterator banIterator = oldBans.begin();
for( ; banIterator != oldBans.end() ; ++banIterator )
	{
	sqlBan* theBan = *banIterator ;

	bot->UnBan(theChannel, theBan->getBanMask());

	sqlChannel::sqlBanMapType::iterator sqlBanIterator =
		theChan->banList.find( theBan->getID() ) ;
	if( sqlBanIterator == theChan->banList.end() )
		{
		elog	<< "cservice::UNBAN> Unable to locate sqlBan, "
			<< "id: "
			<< theBan->getID()
			<< "mask: "
			<< theBan->getBanMask()
			<< endl ;
		continue ;
		}

	theChan->banList.erase( sqlBanIterator );
	theBan->deleteRecord();
	delete(theBan);
	banCount++;
	}
	
/*
 *  Scan through the channel banlist too, and attempt to match any.
 */

Channel::const_banIterator cPtr = theChannel->banList_begin();
while (cPtr != theChannel->banList_end())
	{
	if ( isNick )
		{
		comparison = match((*cPtr), banTarget);
		}
	else if (!isCIDR)
		{
		comparison = match(banTarget, (*cPtr));
		}

	if (exactmatch == 1)
		{		
		/* if we matched exactly above, we want to match exactly here too */
		comparison = strcasecmp((*cPtr), banTarget);
		}

	if ( comparison == 0)
		{
		// Can't call xClient::UnBan inside the loop it will
		// modify without a return value.
		stringstream s;
		s	<< bot->getCharYYXXX()
			<< " M " << theChannel->getName()
			<< " -b " << (*cPtr)
			<< ends;
		bot->Write( s );

		theChannel->removeBan(*cPtr);
		cPtr = theChannel->banList_begin();
		}
	else
		{
		++cPtr;
		}

	} // while()

bot->Notice(theClient,
	bot->getResponse(theUser,
		language::bans_removed,
		string("Removed %i bans that matched %s")).c_str(),
	banCount, banTarget.c_str());

return true;

}

} // namespace gnuworld.

