/* FORCECommand.cc */

#include	<string>
#include	<utility>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char FORCECommand_cc_rcsId[] = "$Id: FORCECommand.cc,v 1.14 2002/10/19 19:53:59 gte Exp $" ;

namespace gnuworld
{
using namespace gnuworld;

bool FORCECommand::Exec( iClient* theClient, const string& Message )
{
	bot->incStat("COMMANDS.FORCE");

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
	if (!theUser) return false;

	sqlChannel* admChan = bot->getChannelRecord("*");

	int admLevel = bot->getAccessLevel(theUser, admChan);
	if (admLevel < level::force)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::insuf_access,
				string("Sorry, you have insufficient access to perform that command.")));
		return false;
	}

 	/*
	 *  First, check the channel is registered/real.
	 */

	if ( (st[1][0] != '#') )
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::inval_chan_name,
				string("Invalid channel name.")));
		return false;
	}

	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::chan_not_reg,
				string("Sorry, %s isn't registered with me.")).c_str(),
			st[1].c_str());
		return false;
	}

 	/*
	 * Add an entry to this channel records 'Force List'.
	 * This list is checked in getEffectiveAccess(), and will return
	 * the relevant amount of access the admin has forced too.
	 */

	theChan->forceMap.insert(sqlChannel::forceMapType::value_type(
		theUser->getID(),
		std::make_pair(admLevel, theUser->getUserName())
	));

	bot->logAdminMessage("%s (%s) is getting access on %s",
		theClient->getNickName().c_str(), theUser->getUserName().c_str(), theChan->getName().c_str());
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::temp_inc_access,
			string("Temporarily increased your access on channel %s to %i")).c_str(),
		theChan->getName().c_str(), admLevel);
	bot->writeChannelLog(theChan, theClient, sqlChannel::EV_FORCE, "");

	return true;
}

} // namespace gnuworld.
