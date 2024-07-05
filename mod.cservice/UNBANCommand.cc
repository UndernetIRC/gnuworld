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
#include	"banMatcher.h"

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

StringTokenizer st2( st[2], ',' ) ;
StringTokenizer::size_type counter = 0 ;

//int max_multibans = bot->getConfigVar("MAX_MULTIBANS")->asInt();
int max_multiunbans = 12 ;

if( (int)st2.size() > max_multiunbans )
	{
	bot->Notice( theClient, "Sorry, you can only remove %i bans with one UNBAN command.",
			max_multiunbans ) ;
	return false ;
	}

// Let's store masks to unban in a vector.
std::vector< string > unbanList ;

/* Strings for providing colapsed responses. */
string respUnban ;
string respNotOnChan ;

for( ; counter < st2.size() ; counter++ )
	{
	elog	<< "cservice::UNBANCommand> #" << counter
				<< ": " << st2[counter]
				<< endl;

	/*
	 *  Are they trying to unban by nick or hostmask?
	 */
	bool isNick = validUserMask(fixAddress(st2[counter])) ? false : true ;

	/* Try by nickname first, remove any bans that match this users host */
	string banTarget ;
	if( isNick )
		{
		iClient* aNick = Network->findNick(st2[counter]);
		if(!aNick)
			{
			/* Adding specified nick to response string. */
			if( !respNotOnChan.empty() )
				respNotOnChan += ", ";

			respNotOnChan += st2[counter] ;

			continue ;
			}

		banTarget = Channel::createBan(aNick);
		}
	else
		{
		banTarget = fixAddress(st2[counter]);
		}

	/* Adding specified ban to response string. */
	if( !respUnban.empty() )
		respUnban += ", ";

	respUnban += banTarget ;

	/*
	 *  Loop over all bans, removing any that match our target
	 */
	std::map< int,sqlBan* >::iterator ptr = theChan->banList.begin();

	bool comparison = false;
	bool exactmatch = false;
	vector<sqlBan*> oldBans;

	while (ptr != theChan->banList.end())
		{
		sqlBan* theBan = ptr->second;
		/*
		 * If we're matching by a users full host, reverse the way we check
		 * banmask.
		 */

		/* do a (case insensitive) literal match */
		exactmatch = !(strcasecmp(theBan->getBanMask(), banTarget));

		if ( isNick )
			{
			comparison = !(match(theBan->getBanMask(), banTarget));
			}
		else
			{
			comparison = banMatch(banTarget, theBan->getBanMask());
			}

		if ( comparison )
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
				if (exactmatch)
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
		}

	/*
	 *  Scan through the channel banlist too, and attempt to match any.
	 */
	Channel::const_banIterator cPtr = theChannel->banList_begin();
	while (cPtr != theChannel->banList_end())
		{
		if ( isNick )
			{
			comparison = !(match((*cPtr), banTarget));
			}
		else
			{
			comparison = banMatch(banTarget, (*cPtr));
			}

		if (exactmatch)
			{
			/* if we matched exactly above, we want to match exactly here too */
			comparison = !(strcasecmp((*cPtr), banTarget));
			}

		if ( comparison )
			{
			unbanList.push_back( *cPtr ) ;
			theChannel->removeBan(*cPtr);
			cPtr = theChannel->banList_begin();
			}
		else
			{
			++cPtr;
			}

		} // while()
	} // for()

string modeString ;
string args ;

for( std::vector< string >::const_iterator ptr = unbanList.begin(),
end = unbanList.end() ; ptr != end ; ++ptr )
	{
	modeString += 'b' ;
	args += *ptr + ' ' ;

	if( ( MAX_CHAN_MODES == modeString.size() ) ||
		( ( ptr + 1 ) == end ) )
		{
		stringstream s ;
		s	<< bot->getCharYYXXX() << " M "
			<< theChan->getName() << ' '
			<< "-" << modeString << ' ' << args ;

		bot->Write( s ) ;

		modeString.erase( modeString.begin(), modeString.end() ) ;
		args.erase( args.begin(), args.end() ) ;
		} // if()
	} // for()

if( !respNotOnChan.empty() )
	bot->Notice( theClient,
		bot->getResponse( theUser,
			language::dont_see_them,
			string( "I don't see %s anywhere" ) ).c_str(),
			respNotOnChan.c_str() ) ;

bot->Notice( theClient,
	bot->getResponse( theUser,
		language::bans_removed,
		string( "Removed %i bans that matched %s" ) ).c_str(),
		unbanList.size(), respUnban.c_str() ) ;

// Send action opnotice to channel if OPLOG is enabled
if( theChan->getFlag( sqlChannel::F_OPLOG ) )
	bot->NoticeChannelOps( theChan->getName(),
		"%s (%s) removed %i bans that matched %s",
		theClient->getNickName().c_str(), theUser->getUserName().c_str(),
		unbanList.size(), respUnban.c_str() ) ;

return true;

}

} // namespace gnuworld.

