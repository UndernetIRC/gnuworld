/*
 * SCANHOSTCommand.cc
 *
 * 08/10/2002 - Jochen Meesters <ekips@pandora.be>
 * Initial Version.
 *
 * Displays User name, and last hostmask
 * for users matching the search request.
 *
 */


#include        <string>
#include        <sstream>

#include        "StringTokenizer.h"
#include        "cservice.h"
#include        "levels.h"
#include        "responses.h"
#include        "libpq++.h"


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
scanhostQuery << "SELECT users.user_name, users_lastseen.last_hostmask FROM users, users_lastseen WHERE "
                << "users.id = users_lastseen.user_id AND "
		<< "lower(users_lastseen.last_hostmask) LIKE '" << escapeSQLChars(searchSQL(host)) << "' LIMIT 50"
                << ends;

ExecStatusType status = bot->SQLDb->Exec( scanhostQuery.str().c_str() ) ;

if( PGRES_TUPLES_OK != status )
        {
        elog    << "SCANHOST> SQL Error: "
                << bot->SQLDb->ErrorMessage()
                << endl ;
        return false ;
        }

bot->Notice(theClient,"Found %i matches", bot->SQLDb->Tuples());

if( bot->SQLDb->Tuples() >= 50 )
	{
	bot->Notice(theClient, "More then 50 matches were found, please visit the website.");
	return false;
	}

if((bot->SQLDb->Tuples() > 15) && (!showAll))
	{
	bot->Notice(theClient, "More then 15 matches were found without the use of -all, please visit the website.");
	return false;
	}

for(int i = 0; i < bot->SQLDb->Tuples(); i++)
        {
        string username = bot->SQLDb->GetValue(i, 0);
        string lasthost = bot->SQLDb->GetValue(i, 1);
        bot->Notice(theClient, "Username: %s -- Last hostmask: %s", username.c_str(), lasthost.c_str());
        }
return true;
}
}
