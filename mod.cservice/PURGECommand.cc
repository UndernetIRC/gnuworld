/**
 * PURGECommand.cc
 *
 * 24/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Purges a channel.
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
 * $Id: PURGECommand.cc,v 1.22 2008/04/16 20:34:40 danielaustin Exp $
 */

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
#include	"cservice_config.h"

namespace gnuworld
{
using std::endl ;
using std::ends ;
using std::string ;
using std::stringstream ;

bool PURGECommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.PURGE");

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
if (!theUser)
	{
	return false;
	}

/*
 *  First, check the channel isn't already registered.
 */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if ((!theChan) || (st[1] == "*"))
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::chan_not_reg,
			string("%s isn't registered with me")).c_str(),
		st[1].c_str());
	return false;
	}

/*
 *  Check the user has sufficient access for this command..
 */

int level = bot->getAdminAccessLevel(theUser);
if (level < level::purge)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("You have insufficient access to perform that command")));
	return false;
	}

/*
 * Don't purge the channel if NOPURGE is set.
 */

if(theChan->getFlag(sqlChannel::F_NOPURGE))
{
	bot->Notice(theClient, "%s has NOPURGE set, so I'm not purging it.",
		theChan->getName().c_str());
	return false;
}

bool reop;
string reason;

if (!strcasecmp(st[2],"-noop"))
{
	reason = st.assemble(3);
	reop = false;
} else {
	reason = st.assemble(2);
	reop = true;
}

/*
 * Fetch some information about the owner of this channel, so we can
 * 'freeze' it for future investigation in the log.
 */

stringstream managerQuery;
managerQuery	<< "SELECT users.user_name,users.email "
		<< "FROM users,levels "
		<< "WHERE levels.user_id = users.id "
		<< "AND levels.access = 500 "
		<< "AND levels.channel_id = "
		<< theChan->getID()
		<< " LIMIT 1"
		<< ends;

string manager = "No Manager";
string managerEmail = "No Email Address";

if( !bot->SQLDb->Exec(managerQuery, true ) )
	{
	LOG( ERROR, "PURGECommand SQL Error:") ;
	LOGSQL_ERROR( bot->SQLDb ) ;
	return false ;
	}
else
	{
		if (bot->SQLDb->Tuples() != 0)
		{
			manager = bot->SQLDb->GetValue(0,0);
			managerEmail = bot->SQLDb->GetValue(0,1);
		}
	}

/* If we need to reop, do it here */
if (reop)
{
	/* iterate over the channel userlist */
	vector< iClient* > opList;
	Channel* tmpChan = Network->findChannel(theChan->getName());
	/* only parse the following if the channel exists on the network
	 * if there is nobody in the channel, there is nobody to op.
	 */
	if (tmpChan)
	{
		for (Channel::userIterator chanUsers = tmpChan->userList_begin();
			chanUsers != tmpChan->userList_end(); ++chanUsers)
		{
			ChannelUser* tmpUser = chanUsers->second;
			iClient* tmpClient = tmpUser->getClient();
			sqlUser* tUser = bot->isAuthed(tmpClient, false);
			if (!tUser)
				continue;
			/* are they globally suspended? */
			if (tUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
				continue;
			sqlLevel* theLevel = bot->getLevelRecord(tUser, theChan);
			/* check if they have access (and not suspended) */
			if (theLevel)
			{
				if (theLevel->getSuspendExpire() > bot->currentTime())
					continue;
				if (theLevel->getAccess() >= 100)
				{
					/* they're 100+, op them */                        
					opList.push_back(tmpClient);
				}
			}
		}
		/* actually do the ops */
		if (!opList.empty())
		{
			/* check we are in the channel, and opped */
			ChannelUser* tmpBotUser = tmpChan->findUser(bot->getInstance());
			if (tmpBotUser)
			{
				if (!tmpBotUser->getMode(ChannelUser::MODE_O))
				{
					/* op ourselves so that we can do the reops */
					stringstream s;
					s	<< bot->getCharYY()
						<< " M "
						<< theChan->getName()
						<< " +o "
						<< bot->getCharYYXXX()
						<< ends;
					bot->Write( s );
					/* update the channel state */
					tmpBotUser->setMode(ChannelUser::MODE_O);
				}
			}
			/* do the ops - if we were not in the channel before, this will
			 * auto-join and op the bot anyway.
			 */
			bot->Op(tmpChan, opList);
		}
	}
}

bot->doXQOplist(theChan->getName());

/*
 * Reset everything back to nice default values.
 */

theChan->clearFlags();
theChan->setMassDeopPro(3);
theChan->setFloodPro(0);
theChan->setURL("");
theChan->setDescription("");
theChan->setComment("");
theChan->setKeywords("");
theChan->setRegisteredTS(0);
theChan->setChannelMode("+tn");
theChan->commit();

bot->logAdminMessage("%s (%s) has purged %s (%s)",
	theClient->getNickName().c_str(),
	theUser->getUserName().c_str(),
	theChan->getName().c_str(),
	reason.c_str());

bot->Notice(theClient,
	bot->getResponse(theUser, language::purged_chan,
		string("Purged channel %s")).c_str(),
	st[1].c_str());

bot->writeChannelLog(theChan,
	theClient,
	sqlChannel::EV_PURGE,
	"has purged " + theChan->getName() + " (" + reason + "), " +
	"Manager was " + manager + " (" + managerEmail + ")" );

/* Notify services - currently disabled. */
//bot->doXQToAllServices("AnyCServiceRouting", "REMCHAN " + theChan->getName());

/* no longer interested in this channel */
bot->getUplink()->UnRegisterChannelEvent( theChan->getName(), bot ) ;
/* remove mode 'R' (no longer registered) */
Channel* tmpChan = Network->findChannel(theChan->getName());
if (tmpChan)
	bot->getUplink()->Mode(NULL, tmpChan, string("-R"), string() );
bot->Part(theChan->getName());
bot->decrementJoinCount();

bot->removeChannelCache(theChan);
delete(theChan);

return true ;
}

} // namespace gnuworld.
