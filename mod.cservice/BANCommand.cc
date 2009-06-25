/**
 * BANCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 20/01/2001 - David Henriksen <david@itwebnet.dk>
 * BAN Command started.
 * 23/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Cleanups/rewrites - don't allow adding of less specific bans.
 * 10/02/2001 - David Henriksen <david@itwebnet.dk>
 * Minor bug fixes.
 * 01/03/01 - Daniel Simard <svr@undernet.org>
 * Fixed Language module stuff.
 * 2001-03-16: Perry Lorier <isomer@coders.net>
 * Fixed ban's without a reason defaulting to level 75
 *
 * Bans a user on a channel, adds this ban to the internal banlist.
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
 * $Id: BANCommand.cc,v 1.49 2009/06/25 19:05:23 mrbean_ Exp $
 */

#include	<new>
#include	<string>
#include	<sstream>
#include	<iostream>

#include	<cassert>

#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"misc.h"
#include	"responses.h"
#include	"match.h"
#include	"ip.h"
#include	"cidr.h"

const char BANCommand_cc_rcsId[] = "$Id: BANCommand.cc,v 1.49 2009/06/25 19:05:23 mrbean_ Exp $" ;

namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;
using namespace level;

bool BANCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.BAN");

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}


/* Is the user authorised? */
sqlUser* theUser = bot->isAuthed(theClient, true);
if(!theUser)
	{
	return false;
	}

/* Do not allow bans on * channel */

if(st[1][0] != '#')
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::inval_chan_name).c_str());
	return false;
	}

/* Is the channel registered? */
sqlChannel* theChan = bot->getChannelRecord(st[1]);
if(!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(
			theUser,
			language::chan_not_reg).c_str(),
		st[1].c_str());
	return false;
	}


/* Check the bot is in the channel. */

if (!theChan->getInChan())
	{
	bot->Notice(
		theClient,
		bot->getResponse(theUser,
			language::i_am_not_on_chan).c_str()
		);
	return false;
	}

/*
 * Check the channel currently exists on the network, if so - we can
 * start kicking.
 */

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

int oCount = 0;
unsigned int banTime = (3*3600);
int banLevel = 75;
string banReason = "No Reason";

if(st.size() >= 6) oCount = 3;
if(st.size() == 5) oCount = 2;
if(st.size() == 4) oCount = 1;

switch(oCount)
	{
	case 1:
	 	{
		/*
		 *  Just one parameter supplied, its either a duration,
		 *  or we'll also accept a reason and set defaults.
		 */

 		if(!IsTimeSpec(st[3]))
 			{
			banReason = st.assemble(3);
 			}
			else
			{
			banTime = extractTime(st[3], 3600);
			}
		break;
		}
	case 2:
		{
		/*
		 *  Two parameters supplied, it's either reason,
		 *  duration + reason or duration and level.
		 */
		if(!IsTimeSpec(st[3]))
			{
			/* first param is not duration, must be reason */
			banReason = st.assemble(3);
			break;
			}

		if(!IsNumeric(st[4]))
			{
			/* first param IS duration (see above), 2nd param is
			 * NOT level - must be reason
			 */
			banReason = st.assemble(4);
			banTime = extractTime(st[3], 3600);
			break;
			}

		/* if we reach here, first param is duration, 2nd param is level
		 * and no reason is specified.
		 */
		banTime = extractTime(st[3], 3600);
		banLevel = atoi(st[4].c_str());
		break;
		}
	case 3:
		{
		/*
		 *  All parameters supplied, it's either reason, duration +
		 *  reason or duration + level + reason.
		 */
		if(!IsTimeSpec(st[3]))
			{
			/* first param is NOT duration, must be reason */
			banReason = st.assemble(3);
			break;
			}

		if(!IsNumeric(st[4]))
			{
			/* first param IS duration (see above), 2nd param is
			 * NOT level - must be reason.
			 */
			banReason = st.assemble(4);
			banTime = extractTime(st[3], 3600);
			break;
			}

		/* if we reach here, we have duration, level and reason */
		banTime = extractTime(st[3], 3600);
		banLevel = atoi(st[4].c_str());
		banReason = st.assemble(5);
		break;
		}
	} // switch()

// Check level.
int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if(level < level::ban)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access).c_str()
		);
	return false;
	}

// TODO: Violation of the rule of numbers
if(banLevel < 1 || banLevel > level || 500 < banLevel)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,language::ban_level_range).c_str(),
		(500 < level) ? 500 : level);
	return true;
	}

if(banTime < 300 || banTime > bot->getConfigVar("MAX_BAN_DURATION")->asInt())
	{
	int maxbanhours = (bot->getConfigVar("MAX_BAN_DURATION")->asInt() / 3600);
	bot->Notice(theClient,
		bot->getResponse(theUser,
		language::ban_duration).c_str(),
		maxbanhours
	);
	return true;
	}

// TODO: Violation of the rule of numbers
if(banReason.size() > 128)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
		language::ban_reason_size).c_str()
	);
	return true;
	}

/* check if the banlist is full */
int max_bans = 0;
int global_max_bans = bot->getConfigVar("MAX_BANS")->asInt();
int local_max_bans = theChan->getMaxBans();
if (local_max_bans > global_max_bans)
	max_bans = local_max_bans;
else
	max_bans = global_max_bans;
int ban_count = 0;

if (max_bans > 0)
{
	/* if we have a max ban limit, get a count of the bans */
	std::map < int,sqlBan* >::iterator ptr = theChan->banList.begin();

	while (ptr != theChan->banList.end())
	{
		ban_count++;	/* increment ban counter */
		++ptr;
	}

	if (ban_count >= max_bans)
	{
		/* banlist is full */
		bot->Notice(theClient, "Sorry, The channel banlist is full (%i bans)",
			ban_count);
		return true;

	}
}

string banTarget = st[2];

bool isNick = bot->validUserMask( banTarget ) ? false : true ;

if( isNick )
	{
	iClient* aNick = Network->findNick(banTarget);
	if(!aNick)
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
			language::cant_find_on_chan).c_str(),
			st[2].c_str(), theChan->getName().c_str()
		);
		return true;
		}

	/* Ban and kick this user */
	banTarget = Channel::createBan(aNick);
	}

/*
 *  Get a list of all bans on this channel, try and match this ban and
 *  find overlapping bans.
 */

xCIDR cidr = xCIDR(banTarget);

std::map < int,sqlBan* >::iterator ptr = theChan->banList.begin();
vector <sqlBan*> oldBans;

while (ptr != theChan->banList.end())
	{
	sqlBan* theBan = ptr->second;

	if(string_lower(banTarget) == string_lower(theBan->getBanMask()))
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
			language::ban_exists).c_str()
		);
		return true;
		}

	if(!cidr.GetValid())
		{
		/*
		 * Overlapping ban? We just remove the ban from our internal tables, as
		 * setting this ban to ircu will cause a default removal of overlapping
		 * bans.
		 */

		// Matched overlapping ban.
		if(match(banTarget, theBan->getBanMask()) == 0)
			{
			// If we have access to remove the overlapper..
			if (theBan->getLevel() <= level)
				{
				// Update GNUWorld.
				oldBans.push_back(theBan);
				}
			}
		// More specific ban?
		else if ( match(theBan->getBanMask(), banTarget) == 0)
			{
			bot->Notice(theClient,
				bot->getResponse(theUser,
				language::ban_covered).c_str(),
				banTarget.c_str(), theBan->getBanMask().c_str());
			return true;
			}
		// Carry on regardless.
		} //cidr.GetValid
	++ptr;
	} // while()
/*
 Go over the bans that needs to be removed
 and remove them
 */
 
vector<sqlBan*>::iterator banIterator = oldBans.begin();
for( ; banIterator != oldBans.end() ; ++banIterator )
	{
	sqlBan* theBan = *banIterator;

	// First, remove the ban from the gnuworld::Channel
	if( !theChannel->removeBan(theBan->getBanMask()) )
		{
		elog	<< "cservice::BANCommand> Unable to find "
			<< "ban: "
			<< theBan->getBanMask()
			<< endl ;
		continue ;
		}

	// Lookup the ban in the sqlBan table in mod.cservice
	sqlChannel::sqlBanMapType::iterator sqlBanIterator =
		theChan->banList.find( theBan->getID() ) ;
	if( sqlBanIterator == theChan->banList.end() )
		{
		elog	<< "cservice::BANCommand> Unable to find "
			<< "ban in sqlChannel, id "
			<< theBan->getID()
			<< ", mask: "
			<< theBan->getBanMask()
			<< endl ;
		continue ;
		}

	// Erase the sqlBan from the cservice ban table
	theChan->banList.erase( sqlBanIterator );

	// Erase the ban from the database
	theBan->deleteRecord();

	// Free allocated memory
	delete(theBan); theBan = 0 ;
	}

// TODO: Violation of rule of numbers
if (banLevel == 42)
	{
	// TODO: Perhaps put this into the .conf
	banReason = "..I'll have a pan-galactic gargleblaster please!";
 	}

/*
 *  Fill out new ban details.
 */

sqlBan* newBan = new (std::nothrow) sqlBan(bot->SQLDb);
assert( newBan != 0 ) ;

// TODO: Use a decent constructor for this
newBan->setChannelID(theChan->getID());
newBan->setBanMask(banTarget);
newBan->setSetBy(theUser->getUserName());
newBan->setSetTS(bot->currentTime());
newBan->setLevel(banLevel);
newBan->setExpires(banTime+bot->currentTime());
newBan->setReason(banReason);

vector< iClient* > clientsToKick ;

for(Channel::userIterator chanUsers = theChannel->userList_begin();
	chanUsers != theChannel->userList_end(); ++chanUsers)
	{
	ChannelUser* tmpUser = chanUsers->second;
	/*
	 *  Iterate over channel members, find a match and boot them..
	 */
	
	if( newBan->getMatcher()->matches(tmpUser->getClient()) )
		{
		/* Don't kick +k things */
		if( !tmpUser->getClient()->getMode(iClient::MODE_SERVICES) )
			{
			clientsToKick.push_back(tmpUser->getClient());
			}
		}
	} // for()


/*
 * If this ban level is < 75, we don't kick the user, we simply don't
 * allow any of the matching hosts to be opped anymore.
 */
// TODO: Violation of rule of numbers
if (banLevel < 75)
	{
	bot->DeOp(theChannel, clientsToKick);
	}
else
	{
	/*
	 *  Otherwise, > 100 bans result in the user being kicked out
	 *  and a ban placed on the channel.
	 */
	string finalReason = "(" + theUser->getUserName() + ") " + banReason;
	if( !clientsToKick.empty() )
		{
			/* ban them */
			bot->Write("%s M %s +b :%s",
				bot->getCharYYXXX().c_str(),
				theChannel->getName().c_str(),
				banTarget.c_str());

			/* add ban to channel banlist */
			theChannel->setBan(banTarget);

			/* kick the users */
			bot->Kick( theChannel, clientsToKick, finalReason ) ;
		}
	}


//theChan->banList[newBan->getID()] = newBan;

/* Insert this new record into the database. */
newBan->insertRecord();

/* Insert to our internal List. */
theChan->banList.insert(std::map<int,sqlBan*>::value_type(newBan->getID(),newBan));

bot->Notice(theClient,
	bot->getResponse(theUser, language::ban_added, "Added ban %s to %s at level %i").c_str(),
	newBan->getBanMask().c_str(),
	theChannel->getName().c_str(),
	newBan->getLevel());

return true ;
}

} // Namespace GNUWorld.
