/*
 * UNBANCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Unbans a user from a channel.
 *
 * Caveats: None.
 *
 * $Id: UNBANCommand.cc,v 1.13 2002/02/06 18:56:25 gte Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"
#include	"match.h"

const char UNBANCommand_cc_rcsId[] = "$Id: UNBANCommand.cc,v 1.13 2002/02/06 18:56:25 gte Exp $" ;

namespace gnuworld
{
using std::string ;
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

vector< sqlBan* >::iterator ptr = theChan->banList.begin();
string banTarget ;

/*
 *  Are they trying to unban by nick or hostmask?
 */
bool isNick = bot->validUserMask( st[2] ) ? false : true ;

/* Try by nickname first, remove any bans that match this users host */

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
	}

/*
 *  Loop over all bans, removing any that match our target
 */

size_t banCount = 0;
unsigned short comparison = 0;

while (ptr != theChan->banList.end())
	{
	sqlBan* theBan = *ptr;
	/*
	 * If we're matching by a users full host, reverse the way we check
	 * banmask.
	 */

	if ( isNick )
		{
		comparison = match(theBan->getBanMask(), banTarget);
		}
	else
		{
		comparison = match(banTarget, theBan->getBanMask());
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
			++ptr;
			}
		else
			{
			bot->UnBan(theChannel, theBan->getBanMask());
			ptr = theChan->banList.erase(ptr);
			theBan->deleteRecord();
			delete(theBan);
			banCount++;
			}
		} // if (banMatched)
	else
		{
		++ptr;
		}

	} // while()

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
	else
		{
		comparison = match(banTarget, (*cPtr));
		}

	if ( comparison == 0)
		{
		// Can't call xClient::UnBan inside the loop it will modify without
		// a return value.
		strstream s;
		s	<< bot->getCharYYXXX()
			<< " M " << theChannel->getName()
			<< " -b " << (*cPtr)
			<< ends;
		bot->Write( s );
		delete[] s.str();

		theChannel->removeBan(*cPtr);
		cPtr = theChannel->banList_begin();

		banCount++;
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

