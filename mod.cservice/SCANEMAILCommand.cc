/**
 * SCANEMAILCommand.cc
 *
 * 18/03/11 - Jochen Meesters <ekips@pandora.be>
 * Initial Version.
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

bool SCANEMAILCommand::Exec( iClient* theClient, const string& Message )
{

bot->incStat("COMMANDS.SCANEMAIL");

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
if (level < level::scanemail)
        {
        bot->Notice(theClient,
                bot->getResponse(theUser,
                        language::insuf_access,
                        string("You have insufficient access to perform that command")));
        return false;
        }

string email = string_lower(st[1]);

stringstream scanemailQuery;
scanemailQuery << "SELECT user_name, email FROM users WHERE "
                << "lower(email) LIKE '" << escapeSQLChars(searchSQL(email)) << "' LIMIT 50"
                << ends;

#ifdef LOG_SQL
	elog	<< "SCANEMAIL::sqlQuery> "
		<< scanemailQuery.str()
		<< endl;
#endif

if( !bot->SQLDb->Exec( scanemailQuery, true ) )
//if( PGRES_TUPLES_OK != status )
        {
        elog    << "SCANEMAIL> SQL Error: "
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

for (unsigned int i = 0; i < bot->SQLDb->Tuples(); i++)
{
        string username = bot->SQLDb->GetValue(i, 0);
        string email = bot->SQLDb->GetValue(i, 1);
	bot->Notice(theClient, "Username: %s -- E-mail: %s", username.c_str(), email.c_str());
}

return true;
}
}
