/**
 * sqlUser.cc
 *
 * Storage class for accessing user information either from the backend
 * or internal storage.
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
 * $Id: sqlUser.cc,v 1.47 2008/11/12 20:45:42 mrbean_ Exp $
 */

#include	<sstream>
#include	<string>
#include	<iostream>

#include	<cstring>

#include	"ELog.h"
#include	"misc.h"
#include	"sqlUser.h"
#include	"constants.h"
#include	"cservice.h"
#include	"cservice_config.h"

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

const unsigned int sqlUser::EV_SUSPEND		= 1;
const unsigned int sqlUser::EV_UNSUSPEND	= 2;
const unsigned int sqlUser::EV_ADMINMOD		= 3;
const unsigned int sqlUser::EV_MISC		= 4;
const unsigned int sqlUser::EV_COMMENT		= 5;

sqlUser::sqlUser(cservice* _bot)
 : id( 0 ),
   user_name(),
   password(),
   last_seen( 0 ),
   url(),
   language_id( 0 ),
   flags( 0 ),
   last_used( 0 ),
   instantiated_ts( ::time(NULL) ),
   signup_ts( 0 ),
   email(),
   verifdata(),
   last_hostmask(),
   maxlogins(0),
   last_note(0),
   notes_sent(0),
   failed_logins(0),
   failed_login_ts(0),
   scram_record(),
   logger(_bot->getLogger()),
   SQLDb(_bot->SQLDb)
{
}

/*
 *  Load all data for this user from the backend. (Key: userID)
 */

bool sqlUser::loadData(int userID)
{
/*
 *  With the open database handle 'SQLDb', retrieve information about
 *  'userID' and fill our member variables.
 */

LOG( TRACE, "Attempting to load data for user-id: {}", userID ) ;

stringstream queryString;
queryString	<< "SELECT "
		<< sql::user_fields
		<< " FROM users WHERE id = "
		<< userID
		<< ends;

if( SQLDb->Exec(queryString, true ) )
	{
	/*
	 *  If the user doesn't exist, we won't get any rows back.
	 */

	if(SQLDb->Tuples() < 1)
		{
		return (false);
		}

	setAllMembers(0);

	return (true);
	}

return (false);
}

bool sqlUser::loadData(const string& userName)
{
/*
 *  With the open database handle 'SQLDb', retrieve information about
 *  'userID' and fill our member variables.
 */

LOG( TRACE, "Attempting to load data for user-name: {}", userName ) ;

stringstream queryString;
queryString	<< "SELECT "
		<< sql::user_fields
		<< " FROM users WHERE lower(user_name) = '"
		<< escapeSQLChars(string_lower(userName))
		<< "'"
		<< ends;

if( SQLDb->Exec(queryString, true ) )
	{
	/*
	 *  If the user doesn't exist, we won't get any rows back.
	 */

	if(SQLDb->Tuples() < 1)
		{
		return (false);
		}

	setAllMembers(0);

	return (true);
	}

return (false);
}


void sqlUser::setAllMembers(int row)
{
/*
 *  Support function for both loadData's.
 *  Assumes SQLDb contains a valid results set for all user information.
 */

id = atoi(SQLDb->GetValue(row, 0));
user_name = SQLDb->GetValue(row, 1);
password = SQLDb->GetValue(row, 2);
url = SQLDb->GetValue(row, 3);
language_id = atoi(SQLDb->GetValue(row, 4));
flags = atoi(SQLDb->GetValue(row, 5));
last_updated_by = SQLDb->GetValue(row, 6);
last_updated = atoi(SQLDb->GetValue(row, 7));
signup_ts = atoi(SQLDb->GetValue(row, 8));
email = SQLDb->GetValue(row, 9);
maxlogins = atoi(SQLDb->GetValue(row, 10));
verifdata = SQLDb->GetValue(row, 11);
failed_logins = 0;
failed_login_ts = 0;
totp_key = SQLDb->GetValue(row, 12);
scram_record = SQLDb->GetValue(row, 13);
/* Fetch the "Last Seen" time from the users_lastseen table. */

}

bool sqlUser::commit(iClient* who)
{
if(who)
	return commit(who->getNickUserHost());
else
	return commit("Marvin, the paranoid android.");
}

bool sqlUser::commit(std::string last_updated_by)
{
/*
 *  Build an SQL statement to commit the transient data in this storage class
 *  back into the database.
 */

static const char* queryHeader =    "UPDATE users ";
static const char* queryCondition = "WHERE id = ";

stringstream queryString;
queryString	<< queryHeader
		<< "SET flags = " << flags << ", "
		<< "password = '" << password << "', "
		<< "language_id = " << language_id << ", "
		<< "maxlogins = " << maxlogins << ", "
		<< "last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int, "
		<< "last_updated_by = '" << escapeSQLChars(last_updated_by) << "', "
		<< "totp_key = '" << escapeSQLChars(totp_key) << "', "
		<< "scram_record = '" << escapeSQLChars(scram_record) << "' "
		<< queryCondition << id
		<< ends;

if( !SQLDb->Exec(queryString ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false;
 	}

return true;
}

bool sqlUser::commitLastSeen()
{
/*
 *  Build an SQL statement to write the last_seen field to a seperate table.
 */

static const char* queryHeader =    "UPDATE users_lastseen ";
static const char* queryCondition = "WHERE user_id = ";

stringstream queryString;
queryString	<< queryHeader
		<< "SET last_seen = "
		<< last_seen
		<< ", "
		<< "last_hostmask = '"
		<< escapeSQLChars(last_hostmask)
		<< "', "
		<< "last_ip = '"
		<< escapeSQLChars(last_ip)
		<< "', "
		<< "last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int "
		<< queryCondition
		<< id
		<< ends;

if( !SQLDb->Exec(queryString ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false;
	}

return true;
}

bool sqlUser::commitLastSeenWithoutMask()
{
/*
 *  -- Boy, we need a masked Commit() method in these classes. ;)
 */

static const char* queryHeader =    "UPDATE users_lastseen ";
static const char* queryCondition = "WHERE user_id = ";

stringstream queryString;
queryString	<< queryHeader
		<< "SET last_seen = "
		<< last_seen
		<< ", last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int "
		<< queryCondition
		<< id
		<< ends;

if( !SQLDb->Exec(queryString ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false;
 	}

return true;
}

time_t sqlUser::getLastSeen()
{
stringstream queryString;
queryString	<< "SELECT last_seen"
		<< " FROM users_lastseen WHERE user_id = "
		<< id
		<< ends;

if( SQLDb->Exec(queryString, true ) )
{
	/*
	 *  If the user doesn't exist, we won't get any rows back.
	 */

	if(SQLDb->Tuples() < 1)
	{
		return (false);
	}

	last_seen = atoi(SQLDb->GetValue(0, 0));

	return (last_seen);
}

return (false);

}

const string sqlUser::getLastHostMask()
{
stringstream queryString;
queryString	<< "SELECT last_hostmask"
		<< " FROM users_lastseen WHERE user_id = "
		<< id
		<< ends;

if( SQLDb->Exec(queryString, true ) )
	{
	/*
	 *  If the user doesn't exist, we won't get any rows back.
	 */

	if(SQLDb->Tuples() < 1)
		{
		return ("");
		}

	last_hostmask = SQLDb->GetValue(0, 0);

	return (last_hostmask);
	}

return ("");

}

const string sqlUser::getLastIP()
{
stringstream queryString;
queryString	<< "SELECT last_ip"
		<< " FROM users_lastseen WHERE user_id = "
		<< id
		<< ends;

if( SQLDb->Exec(queryString, true ) )
	{
	/*
	 *  If the user doesn't exist, we won't get any rows back.
	 */

	if(SQLDb->Tuples() < 1)
		{
		return ("");
		}

	last_ip = SQLDb->GetValue(0, 0);

	return (last_ip);
	}

return ("");

}
void sqlUser::writeEvent(unsigned short eventType, sqlUser* theUser, const string& theMessage)
{
string userExtra = theUser ? theUser->getUserName() : "Not Logged In";

stringstream theLog;
theLog	<< "INSERT INTO userlog (ts, user_id, event, message, "
	<< "last_updated) VALUES "
	<< "("
	<< "date_part('epoch', CURRENT_TIMESTAMP)::int"
	<< ", "
	<< id
	<< ", "
	<< eventType
	<< ", "
 	<< "'"
	<< escapeSQLChars(theMessage)
	<< " (By " << userExtra << ")"
	<< "', date_part('epoch', CURRENT_TIMESTAMP)::int)"
	<< ends;

// TODO: Is this ok?
SQLDb->Exec(theLog);
//SQLDb->ExecCommandOk(theLog.str().c_str());
}

const string sqlUser::getLastEvent(unsigned short eventType, unsigned int& eventTime)
{
stringstream queryString;

queryString	<< "SELECT message,ts"
			<< " FROM userlog WHERE user_id = "
			<< id
			<< " AND event = "
			<< eventType
			<< " ORDER BY ts DESC LIMIT 1"
			<< ends;

if( SQLDb->Exec(queryString, true ) )
	{

	if(SQLDb->Tuples() < 1)
		{
		return("");
		}

	string reason = SQLDb->GetValue(0, 0);
	eventTime = atoi(SQLDb->GetValue(0, 1));

	return (reason);
	}

return ("");

}

sqlUser::~sqlUser()
{
// No heap space allocated
}

bool sqlUser::Insert()
{
/*
 * Build an SQL statement to insert the transient data in 
 * this storage class back into the database.
 */
static const char* queryHeader =  "INSERT INTO users "
	"(user_name,password,language_id,flags,last_updated_by,last_"
	"updated,post_forms,signup_ts,email,scram_record) VALUES ('";

stringstream queryString;
queryString	<< queryHeader
		<< escapeSQLChars(user_name)
		<< "'," << "'"
		<< password << "',"
		<< 1 << ","
		<< 0 << ",'"
		<< escapeSQLChars(last_updated_by)
		<< "',"
		<< "date_part('epoch', CURRENT_TIMESTAMP)::int,"
		<< "(date_part('epoch', CURRENT_TIMESTAMP)::int + 432000),"
		<< "date_part('epoch', CURRENT_TIMESTAMP)::int,'"
		<< escapeSQLChars(email)
		<< "','" << escapeSQLChars(scram_record)
		<< "')"
		<< ends;

if( !SQLDb->Exec(queryString ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false;
    }

return true;
}

} // namespace gnuworld.
