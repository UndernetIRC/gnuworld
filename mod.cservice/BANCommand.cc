/*
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
 * $Id: BANCommand.cc,v 1.31 2002/10/16 00:13:40 gte Exp $
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

const char BANCommand_cc_rcsId[] = "$Id: BANCommand.cc,v 1.31 2002/10/16 00:13:40 gte Exp $" ;

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
int banTime = 3;
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

 		if(!IsNumeric(st[3]))
 			{
			banReason = st.assemble(3);
 			}
			else
			{
			banTime = atoi(st[3].c_str());
			}
		break;
		}
	case 2:
		{
		if(!IsNumeric(st[3]))
			{
			banReason = st.assemble(3);
			break;
			}

		if(!IsNumeric(st[4]))
			{
			banReason = st.assemble(4);
			banTime = atoi(st[3].c_str());
			}

		banTime = atoi(st[3].c_str());
		banLevel = atoi(st[4].c_str());
		break;
		}
	case 3:
		{
		if(!IsNumeric(st[3]))
			{
			banReason = st.assemble(3);
			break;
			}

		if(!IsNumeric(st[4]))
			{
			banReason = st.assemble(4);
			banTime = atoi(st[3].c_str());
			break;
			}

		banTime = atoi(st[3].c_str());
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

// TODO: Violation of the rule of numbers
if(banTime < 1 || banTime > 336)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
		language::ban_duration).c_str()
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

int banDuration = banTime * 3600;
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

vector< sqlBan* >::iterator ptr = theChan->banList.begin();

while (ptr != theChan->banList.end())
	{
	sqlBan* theBan = *ptr;

	if(string_lower(banTarget) == string_lower(theBan->getBanMask()))
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
			language::ban_exists).c_str()
		);
		return true;
		}

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
			theChannel->removeBan(theBan->getBanMask());
			ptr = theChan->banList.erase(ptr);
			theBan->deleteRecord();
			delete(theBan);
			}
		else
			{
			++ptr;
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
	else
		{
		++ptr;
		}
	} // while()

vector< iClient* > clientsToKick ;
for(Channel::userIterator chanUsers = theChannel->userList_begin();
	chanUsers != theChannel->userList_end(); ++chanUsers)
	{
	ChannelUser* tmpUser = chanUsers->second;
	/*
	 *  Iterate over channel members, find a match and boot them..
	 */

	if( (match(banTarget, tmpUser->getClient()->getNickUserHost()) == 0) ||
		(match(banTarget, tmpUser->getClient()->getRealNickUserHost()) == 0) )
		{
		/* Don't kick +k things */
		if( !tmpUser->getClient()->getMode(iClient::MODE_SERVICES) )
			{
			clientsToKick.push_back(tmpUser->getClient());
			}
		}
	} // for()

// TODO: Violation of rule of numbers
if (banLevel == 42)
	{
	// TODO: Perhaps put this into the .conf
	banReason = "..I'll have a pan-galactic gargleblaster please!";
 	}

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
		// TODO: Use xClient::Ban() here
		stringstream s;
		s	<< bot->getCharYYXXX() << " M " << theChannel->getName()
			<< " +b " << banTarget << ends;

		bot->Write( s );

		bot->Kick( theChannel, clientsToKick, finalReason ) ;

		/* Update GNUWorld */
		theChannel->setBan(banTarget);
		}
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
newBan->setExpires(banDuration+bot->currentTime());
newBan->setReason(banReason);

/* Insert to our internal List. */
theChan->banList.push_back(newBan);

/* Insert this new record into the database. */
newBan->insertRecord();

bot->Notice(theClient,
	bot->getResponse(theUser, language::ban_added, "Added ban %s to %s at level %i").c_str(),
	newBan->getBanMask().c_str(),
	theChannel->getName().c_str(),
	newBan->getLevel());

return true ;
}

} // Namespace GNUWorld.
