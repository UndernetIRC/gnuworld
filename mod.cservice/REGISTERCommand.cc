/**
 * REGISTERCommand.cc
 *
 * 26/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Registers a channel.
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
 * $Id: REGISTERCommand.cc,v 1.24 2009/07/31 07:29:13 mrbean_ Exp $
 */

#include	<map>
#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"dbHandle.h"
#include	"Network.h"
#include	"responses.h"

const char REGISTERCommand_cc_rcsId[] = "$Id: REGISTERCommand.cc,v 1.24 2009/07/31 07:29:13 mrbean_ Exp $" ;

namespace gnuworld
{
using std::pair ;
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

using namespace gnuworld;

bool REGISTERCommand::Exec( iClient* theClient, const string& Message )
{
	bot->incStat("COMMANDS.REGISTER");

	bool instantReg = false;

	if (bot->RequiredSupporters == 0)
		instantReg = true;

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false;

	/*
	 *  Check the user has sufficient access for this command..
	 */
	int level = bot->getAdminAccessLevel(theUser);
	if ((level < level::registercmd) && (!instantReg))
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::insuf_access,
				string("You have insufficient access to perform that command.")));
		return false;
	}

	StringTokenizer st( Message ) ;

	if (st.size() < 3)
	{
		if (level == 0)
			Usage(theClient);
		else
			bot->Notice(theClient, "SYNTAX: REGISTER <#channel> [username]");
		return true;
	}

 	/*
	 *  First, check the channel isn't already registered.
	 */

	sqlChannel* theChan;
	theChan = bot->getChannelRecord(st[1]);
	if (theChan)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::chan_already_reg,
				string("%s is already registered with me.")).c_str(),
			st[1].c_str());
		return false;
	}

	sqlUser* tmpUser;

	// In case of instant channel registration, the target user can be only the issuer user
	if (instantReg)
		tmpUser = theUser;

	if ((st.size() > 2) && (level))
	{
		tmpUser = bot->getUserRecord(st[2]);
		if (!tmpUser)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::not_registered,
					string("The user %s doesn't appear to be registered.")).c_str(),
			st[2].c_str());
		return true;
		}
	}

	if (!bot->isValidChannel(st[1]))
	{
		if (!bot->validResponseString.empty())
		{
			bot->Notice(theClient, "Cannot register, channel is %s",bot->getCurrentValidResponse().c_str());
			bot->validResponseString.clear();
		}
       	return false;
	}

	string::size_type pos = st[1].find_first_of( ',' ); /* Don't allow comma's in channel names. :) */

	if ( (st[1][0] != '#') || (string::npos != pos))
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::inval_chan_name,
				string("Invalid channel name.")));
		return false;
	}

	/*
	 * Create the new channel and insert it into the cache.
	 * If the channel exists on IRC, grab the creation timestamp
	 * and use this as the channel_ts in the Db.
	 */

	unsigned int channel_ts = 0;
	Channel* tmpChan = Network->findChannel(st[1]);
	channel_ts = tmpChan ? tmpChan->getCreationTime() : ::time(NULL);

	sqlChannel* newChan = new (std::nothrow) sqlChannel(bot->SQLDb);
	newChan->setName(st[1]);
	newChan->setChannelTS(channel_ts);
	newChan->setRegisteredTS(bot->currentTime());
	newChan->setChannelMode("+tnR");
	newChan->setLastUsed(bot->currentTime());
	newChan->setFlag(sqlChannel::F_AUTOJOIN);

 	/*
	 *  If this channel exists in the database (without a registered_ts set),
	 *  then it is currently unclaimed. This register command will
	 *  update the timestamp, and proceed to adduser.
	 */

	stringstream checkQuery;

	checkQuery 	<< "SELECT id FROM channels WHERE "
				<< "registered_ts = 0 AND lower(name) = '"
				<< escapeSQLChars(string_lower(st[1]))
				<< "'"
				<< ends;

#ifdef LOG_SQL
	elog << "sqlQuery> " << checkQuery.str().c_str() << endl;
#endif

	bool isUnclaimed = false;
	if (bot->SQLDb->Exec(checkQuery, true))
//	if ((status = bot->SQLDb->Exec(checkQuery.str().c_str())) == PGRES_TUPLES_OK)
	{
		if (bot->SQLDb->Tuples() > 0) isUnclaimed = true;
	}

	if (isUnclaimed)
	{
		/*
		 *  Quick query to set registered_ts back for this chan.
		 */

		stringstream reclaimQuery;

		reclaimQuery<< "UPDATE channels SET registered_ts = now()::abstime::int4,"
					<< " last_updated = now()::abstime::int4, "
					<< " flags = 0, description = '', url = '', comment = '', keywords = '', channel_mode = '+tnR' "
					<< " WHERE lower(name) = '"
					<< escapeSQLChars(string_lower(st[1]))
					<< "'"
					<< ends;

#ifdef LOG_SQL
		elog << "sqlQuery> " << reclaimQuery.str().c_str() << endl;
#endif

		if (bot->SQLDb->Exec(reclaimQuery))
//		if ((status = 
//bot->SQLDb->Exec(reclaimQuery.str().c_str())) == PGRES_COMMAND_OK)
		{
			bot->logAdminMessage("%s (%s) has registered %s to %s", theClient->getNickName().c_str(),
				theUser->getUserName().c_str(), st[1].c_str(), tmpUser->getUserName().c_str());

			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::regged_chan,
					string("Registered channel %s")).c_str(),
				st[1].c_str());
		} else {
			bot->Notice(theClient, "Unable to update the channel in the database!");
			return false;
		}

	}
	else /* We perform a normal registration. */
	{
		newChan->insertRecord();

		bot->logAdminMessage("%s (%s) has registered %s to %s", theClient->getNickName().c_str(),
			theUser->getUserName().c_str(), st[1].c_str(), tmpUser->getUserName().c_str());
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::regged_chan,
				string("Registered channel %s")).c_str(),
			st[1].c_str());
	}

	/*
	 *  Now add the target chap at 500 in the new channel. To do this, we need to know
	 *  the db assigned channel id of the newly created channel :/
	 */
	stringstream idQuery;

	idQuery 	<< "SELECT id FROM channels WHERE "
				<< "lower(name) = '"
				<< escapeSQLChars(string_lower(st[1]))
				<< "'"
				<< ends;

#ifdef LOG_SQL
	elog << "sqlQuery> " << idQuery.str().c_str() << endl;
#endif

	unsigned int theId = 0;

	if (bot->SQLDb->Exec(idQuery, true))
//	if ((status = bot->SQLDb->Exec(idQuery.str().c_str())) == 
//PGRES_TUPLES_OK)
	{
		if (bot->SQLDb->Tuples() > 0)
		{
			theId = atoi(bot->SQLDb->GetValue(0, 0));
			newChan->setID(theId);
			newChan->setFlag(sqlChannel::F_NOTAKE);
			newChan->setNoTake(1);
			newChan->commit();
			
			bot->sqlChannelCache.insert(cservice::sqlChannelHashType::value_type(newChan->getName(), newChan));
			bot->sqlChannelIDCache.insert(cservice::sqlChannelIDHashType::value_type(newChan->getID(), newChan));
			cservice::pendingChannelListType::iterator ptr = bot->pendingChannelList.find(newChan->getName());
			if (ptr != bot->pendingChannelList.end())
			{
				sqlPendingChannel* pendingChan = ptr->second;
				pendingChan->commit();
				ptr->second = NULL;
				delete(pendingChan);
				bot->pendingChannelList.erase(ptr);
			}
		} else
		{
			/*
			 * If we can't find the channel in the db, something has gone
			 * horribly wrong.
			 */
			return false;
		}

	} else
	{
		return false;
	}

	/*
	 *  Finally, commit a channellog entry.
	 */

	bot->writeChannelLog(newChan, theClient, sqlChannel::EV_REGISTER, "to " + tmpUser->getUserName());

	/*
	 * Create the new manager.
	 */

	sqlLevel* newManager = new (std::nothrow) sqlLevel(bot->SQLDb);
	newManager->setChannelId(newChan->getID());
	newManager->setUserId(tmpUser->getID());
	newManager->setAccess(500);
	newManager->setAdded(bot->currentTime());
	newManager->setAddedBy("(" + theUser->getUserName() + ") " + theClient->getNickUserHost());
	newManager->setLastModif(bot->currentTime());
	newManager->setLastModifBy("(" + theUser->getUserName() + ") " + theClient->getNickUserHost());

	if (!newManager->insertRecord())
		{
			bot->Notice(theClient, "Couldn't automatically add the level 500 Manager, check it doesn't already exist.");
			delete(newManager);
			return (false);
		}

	/*
	 * Insert this new 500 into the level cache.
	 */

	pair<int, int> thePair( newManager->getUserId(), newManager->getChannelId());
	bot->sqlLevelCache.insert(cservice::sqlLevelHashType::value_type(thePair, newManager));

	bot->getUplink()->RegisterChannelEvent(st[1],bot);
	bot->Join(newChan->getName(), string("+tnR"), newChan->getChannelTS(), true);
	newChan->setInChan(true);
	bot->joinCount++;

	//Send a welcome notice to the channel
	if (!bot->welcomeNewChanMessage.empty())
		bot->Notice(newChan->getName(), TokenStringsParams(bot->welcomeNewChanMessage.c_str(), newChan->getName().c_str()).c_str());

	//Set a welcome topic of the new channel, only if the actual topic is empty
#ifdef TOPIC_TRACK
	if (!bot->welcomeNewChanTopic.empty())
	if (tmpChan && tmpChan->getTopic().empty())
		bot->Topic(tmpChan, bot->welcomeNewChanTopic);
#endif

	return true;
}

} // namespace gnuworld.
