/**
 * SCANUNAMECommand.cc
 *
 * 07/10/2002 - Jochen Meesters <ekips@pandora.be>
 * Initial Version.
 *
 * Displays User name and creation ip
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
 * $Id: SCANUNAMECommand.cc,v 1.3 2003/06/28 01:21:20 dan_karrels Exp $
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

bool SCANUNAMECommand::Exec( iClient* theClient, const string& Message )
{

bot->incStat("COMMANDS.SCANUNAME");

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
if (level < level::scanuname)
        {
        bot->Notice(theClient,
                bot->getResponse(theUser,
                        language::insuf_access,
                        string("You have insufficient access to perform that command")));
        return false;
        }

string uname = string_lower(st[1]);

stringstream scanunameQuery;
scanunameQuery << "SELECT user_name, signup_ip FROM users WHERE "
                << "lower(user_name) LIKE '" << escapeSQLChars(searchSQL(uname)) << "' LIMIT 50"
                << ends;

ExecStatusType status = bot->SQLDb->Exec( scanunameQuery.str().c_str() ) ;

if( PGRES_TUPLES_OK != status )
        {
        elog    << "SCANUNAME> SQL Error: "
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
        string signupip = bot->SQLDb->GetValue(i, 1);
        bot->Notice(theClient, "Username: %s -- Signup ip: %s", username.c_str(), signupip.c_str());
        }
return true;
}
}
