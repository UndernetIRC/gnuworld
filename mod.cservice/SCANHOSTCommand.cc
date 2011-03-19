/**
 * SCANHOSTCommand.cc
 *
 * 08/10/2002 - Jochen Meesters <ekips@pandora.be>
 * Initial Version.
 *
 * Displays User name, and last hostmask
 * for users matching the search request.
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
 * $Id: SCANHOSTCommand.cc,v 1.7 2008/11/12 20:45:42 mrbean_ Exp $
 */


#include        <string>
#include        <sstream>

#include        "StringTokenizer.h"
#include        "cservice.h"
#include        "levels.h"
#include        "responses.h"
#include        "dbHandle.h"


namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

bool SCANHOSTCommand::Exec( iClient* theClient, const string& Message )
{

bot->incStat("COMMANDS.SCANHOST");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
        {
        Usage(theClient);
        return true;
        }

bool showAll = false;

if(st.size() > 2)
	if(!strcasecmp(st[2],"-all"))
		showAll = true;

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
 *  Check the user has sufficient access for this command..
 */

int level = bot->getAdminAccessLevel(theUser);
if (level < level::scanhost)
        {
        bot->Notice(theClient,
                bot->getResponse(theUser,
                        language::insuf_access,
                        string("You have insufficient access to perform that command")));
        return false;
        }

string host = string_lower(st[1]);

stringstream scanhostQuery;
scanhostQuery << "SELECT users.user_name, users_lastseen.last_hostmask, users_lastseen.last_ip FROM users, users_lastseen WHERE "
                << "users.id = users_lastseen.user_id AND "
		<< "(lower(users_lastseen.last_hostmask) LIKE '" << escapeSQLChars(searchSQL(host)) << "'"
		<< " OR lower(users_lastseen.last_ip) LIKE '" << escapeSQLChars(searchSQL(host)) << "') LIMIT 50"
                << ends;

#ifdef LOG_SQL
	elog	<< "SCANHOST::sqlQuery> "
		<< scanhostQuery.str()
		<< endl;
#endif

if( !bot->SQLDb->Exec( scanhostQuery, true ) )
//if( PGRES_TUPLES_OK != status )
        {
        elog    << "SCANHOST> SQL Error: "
                << bot->SQLDb->ErrorMessage()
                << endl ;
        return false ;
        }

bot->Notice(theClient,"Found %i matches", bot->SQLDb->Tuples());

if( bot->SQLDb->Tuples() >= 50 )
	{
	bot->Notice(theClient, "More than 50 matches were found, please visit the website.");
	return false;
	}

if((bot->SQLDb->Tuples() > 15) && (!showAll))
	{
	bot->Notice(theClient, "More than 15 matches were found without the use of -all, please visit the website.");
	return false;
	}

/* use this to store the SQL result set (querying later would overwrite the results */
typedef std::map< std::string, std::string > scanResultsType;
scanResultsType scanResults;
/* counter for matches not displayed */
int matchCount = 0;

/* store the results in the map defined above */
for (unsigned int i = 0; i < bot->SQLDb->Tuples(); i++)
{
	string username = bot->SQLDb->GetValue(i, 0);
	string lasthost = bot->SQLDb->GetValue(i, 1) + " - Last IP: " + bot->SQLDb->GetValue(i, 2);
	scanResults.insert( std::make_pair(username, lasthost));
}

/* use this for each user record (below) */
unsigned short tmpadminLevel;
/* iterate through the results, fetching user records for each */
for (scanResultsType::const_iterator Itr = scanResults.begin();
	Itr != scanResults.end(); ++Itr)
{
	string username = Itr->first;
	string lasthost = Itr->second;

	/* check each user's access for purposes of IP hiding ONLY */
	sqlUser* tmpUser = bot->getUserRecord(username);
	if (tmpUser)
	{
		/* found user, fetch admin access level */
		sqlChannel* adminChan = bot->getChannelRecord("*");
		if (!adminChan)
		{
			/* cant find admin channel, assume no access */
			tmpadminLevel = 0;
		} else {
			/* found admin channel, try to get the level record */
			sqlLevel* adminLev = bot->getLevelRecord(tmpUser, adminChan);
			if (!adminLev)
			{
				/* no level record, assume no access */
				tmpadminLevel = 0;
			} else {
				/* found level record, set it */
				tmpadminLevel = adminLev->getAccess();
			}
		}

		if ((tmpadminLevel>0 || tmpUser->getFlag(sqlUser::F_OPER)) && level<800)
		{
			/* increment counter of items not listed */
			matchCount++;
		} else {
			/* display entry */
			bot->Notice(theClient, "Username: %s -- Last hostmask: %s",
				username.c_str(), lasthost.c_str());
		}
        }
}

/* if we have not displayed any users due to IP hiding, explain here */
if (matchCount > 0)
{
	bot->Notice(theClient, "-- %d staff accounts matched this search, but were not listed",
		matchCount);
}

/* clean up */
scanResults.clear();

return true;
}
}
