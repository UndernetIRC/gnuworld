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
 * $Id: PURGECommand.cc,v 1.16 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"libpq++.h"
#include	"Network.h"
#include	"responses.h"
#include	"cservice_config.h"

const char PURGECommand_cc_rcsId[] = "$Id: PURGECommand.cc,v 1.16 2003/06/28 01:21:20 dan_karrels Exp $" ;

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

#ifdef LOG_SQL
	elog	<< "sqlQuery> "
		<< managerQuery.str().c_str()
		<< endl;
#endif

ExecStatusType status = bot->SQLDb->Exec(managerQuery.str().c_str()) ;

string manager = "No Manager";
string managerEmail = "No Email Address";

if( status != PGRES_TUPLES_OK )
	{
	elog	<< "PURGE> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
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

/*
 * Reset everything back to nice default values.
 */

theChan->clearFlags();
theChan->setMassDeopPro(3);
theChan->setFloodPro(7);
theChan->setURL("");
theChan->setDescription("");
theChan->setComment("");
theChan->setKeywords("");
theChan->setRegisteredTS(0);
theChan->setChannelMode("+tn");
theChan->commit();

/*
 * Permanently delete all associated Level records for this channel.
 */

stringstream theQuery ;

theQuery	<< "DELETE FROM levels WHERE channel_id = "
			<< theChan->getID()
			<< ends;

#ifdef LOG_SQL
elog	<< "sqlQuery> "
		<< theQuery.str().c_str()
		<< endl;
#endif

status = bot->SQLDb->Exec(theQuery.str().c_str()) ;

if( status != PGRES_COMMAND_OK )
	{
	elog	<< "PURGE> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

/*
 * Bin 'em all.
 */

cservice::sqlLevelHashType::const_iterator ptr = bot->sqlLevelCache.begin();
cservice::sqlLevelHashType::key_type thePair;

while(ptr != bot->sqlLevelCache.end())
{
	sqlLevel* tmpLevel = ptr->second;
	unsigned int channel_id = ptr->first.second;

	if (channel_id == theChan->getID())
	{
		thePair = ptr->first;
		elog << "Purging Level Record for: " << thePair.second << " (UID: " << thePair.first << ")" << endl;

		++ptr;
		bot->sqlLevelCache.erase(thePair);

		delete(tmpLevel);
	} else
	{
		++ptr;
	}

}

string reason = st.assemble(2);

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

/* Remove from cache.. part channel. */
bot->sqlChannelCache.erase(theChan->getName());
bot->sqlChannelIDCache.erase(theChan->getID());
bot->getUplink()->UnRegisterChannelEvent( theChan->getName(), bot ) ;
bot->Part(theChan->getName());
bot->joinCount--;

delete(theChan);

return true ;
}

} // namespace gnuworld.
