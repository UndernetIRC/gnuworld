/**
 * REMOVEALLCommand.
 *
 * Quickly clear a channels access list.
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
 * $Id: REMOVEALLCommand.cc,v 1.7 2007/08/28 16:10:11 dan_karrels Exp $
 */

#include	<map>
#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include 	"responses.h"
#include	"levels.h"
#include	"cservice_config.h"

const char REMOVEALLCommand_cc_rcsId[] = "$Id: REMOVEALLCommand.cc,v 1.7 2007/08/28 16:10:11 dan_karrels Exp $" ;

namespace gnuworld
{
using std::pair ;
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

bool REMOVEALLCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.REMOVEALL");

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
if (!theUser)
	{
	return false;
	}

/*
 *  First, check the channel is registered.
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
if (level < level::removeall)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("You have insufficient access to perform that command")));
	return false;
	}


/*
 *  Now, perform a fast query on the levels table for this channel.
 */

stringstream clearAllQuery;
clearAllQuery	<< "SELECT user_id FROM levels WHERE"
		<< " channel_id = "
		<< theChan->getID()
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlQuery> "
		<< clearAllQuery.str().c_str()
		<< endl;
#endif

if( !bot->SQLDb->Exec(clearAllQuery, true ) )
//if( status != PGRES_TUPLES_OK )
	{
	elog	<< "REMOVEALL> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

/*
 * Iterate over all the results, checking if this access is in the
 * cache.
 * If so, remove it!
 */

int delCounter = bot->SQLDb->Tuples();

for (unsigned int i = 0 ; i < bot->SQLDb->Tuples(); i++)
{
	pair<int, int> thePair( atoi(bot->SQLDb->GetValue(i, 0)), theChan->getID() );

	cservice::sqlLevelHashType::iterator ptr = bot->sqlLevelCache.find(thePair);
	if(ptr != bot->sqlLevelCache.end())
		{
		/*
		 *  Found it in the cache, free the memory and
		 *  remove it from the cache.
		 */

		delete(ptr->second);
		bot->sqlLevelCache.erase(thePair);
		}
}

/*
 * Now the cache is clean, execute some SQL to eradicate the records
 * from the database.
 */

stringstream deleteAllQuery;
deleteAllQuery	<< "DELETE FROM levels WHERE"
		<< " channel_id = "
		<< theChan->getID()
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlQuery> "
		<< deleteAllQuery.str().c_str()
		<< endl;
#endif

if (bot->SQLDb->Exec(deleteAllQuery))
	{
		bot->Notice(theClient, "Done. Zapped %i access records from %s",
			delCounter, theChan->getName().c_str());
		bot->writeChannelLog(theChan,
			theClient,
			sqlChannel::EV_REMOVEALL, "" );
	} else
	{
		bot->Notice(theClient, "A database error occured while removing the access records.");
		bot->Notice(theClient, "Please contact a database administrator!");
	}

return true;
}
} // namespace gnuworld.

