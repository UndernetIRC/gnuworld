/*
 * sqlUser.cc
 *
 * Storage class for accessing user information either from the backend
 * or internal storage.
 *
 * $Id: sqlUser.cc,v 1.28 2001/10/07 23:22:17 gte Exp $
 */

#include	<strstream.h>
#include	<string.h>

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

const sqlUser::flagType sqlUser::F_GLOBAL_SUSPEND =	0x01 ;
const sqlUser::flagType sqlUser::F_LOGGEDIN =		0x02 ;
const sqlUser::flagType sqlUser::F_INVIS =			0x04 ;

const unsigned int sqlUser::EV_SUSPEND		= 1;
const unsigned int sqlUser::EV_UNSUSPEND	= 2;
const unsigned int sqlUser::EV_COMMENT		= 3;

sqlUser::sqlUser(PgDatabase* _SQLDb)
 : networkClient(0),
   id( 0 ),
   user_name(),
   password(),
   last_seen( 0 ),
   url(),
   language_id( 0 ),
   flags( 0 ),
   last_used( 0 ),
   email(),
   last_hostmask(),
   SQLDb( _SQLDb )
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

#ifdef LOG_DEBUG
	elog	<< "sqlUser::loadData> Attempting to load data for user-id: "
		<< userID
		<< endl;
#endif

strstream queryString;
queryString	<< "SELECT "
		<< sql::user_fields
		<< " FROM users WHERE id = "
		<< userID
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlUser::loadData> "
		<< queryString.str()
		<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_TUPLES_OK == status )
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

#ifdef LOG_DEBUG
	elog	<< "sqlUser::loadData> Attempting to load data for user-name: "
		<< userName
		<< endl;
#endif

strstream queryString;
queryString	<< "SELECT "
		<< sql::user_fields
		<< " FROM users WHERE lower(user_name) = '"
		<< escapeSQLChars(string_lower(userName))
		<< "'"
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlUser::loadData> "
		<< queryString.str()
		<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_TUPLES_OK == status )
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
email = SQLDb->GetValue(row, 8);

/* Fetch the "Last Seen" time from the users_lastseen table. */

}

bool sqlUser::commit()
{
/*
 *  Build an SQL statement to commit the transient data in this storage class
 *  back into the database.
 */

static const char* queryHeader =    "UPDATE users ";
static const char* queryCondition = "WHERE id = ";

strstream queryString;
queryString	<< queryHeader
		<< "SET flags = " << flags << ", "
		<< "password = '" << password << "', "
		<< "language_id = " << language_id << ", "
		<< "last_updated = now()::abstime::int4 "
		<< queryCondition << id
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlUser::commit> "
		<< queryString.str()
		<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_COMMAND_OK != status )
	{
	// TODO: Log to msgchan here.
	elog	<< "sqlUser::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;

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

strstream queryString;
queryString	<< queryHeader
		<< "SET last_seen = "
		<< last_seen
		<< ", "
		<< "last_hostmask = '"
		<< escapeSQLChars(last_hostmask)
		<< "', "
		<< "last_updated = now()::abstime::int4 "
		<< queryCondition
		<< id
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlUser::commitLastSeen> "
		<< queryString.str()
		<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_COMMAND_OK != status )
	{
	// TODO: Log to msgchan here.
	elog	<< "sqlUser::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;

	return false;
 	}

return true;
}

time_t sqlUser::getLastSeen()
{
strstream queryString;
queryString	<< "SELECT last_seen"
		<< " FROM users_lastseen WHERE user_id = "
		<< id
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlUser::getLastSeen> "
		<< queryString.str()
		<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_TUPLES_OK == status )
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
strstream queryString;
queryString	<< "SELECT last_hostmask"
		<< " FROM users_lastseen WHERE user_id = "
		<< id
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlUser::getLastHostMask> "
		<< queryString.str()
		<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_TUPLES_OK == status )
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


void sqlUser::writeEvent(unsigned short eventType, sqlUser* theUser, const string& theMessage)
{
string userExtra = theUser ? theUser->getUserName() : "Not Logged In";

strstream theLog;
theLog	<< "INSERT INTO userlog (ts, user_id, event, message, "
	<< "last_updated) VALUES "
	<< "("
	<< "now()::abstime::int4"
	<< ", "
	<< id
	<< ", "
	<< eventType
	<< ", "
 	<< "'"
	<< escapeSQLChars(theMessage)
	<< " (By " << userExtra << ")"
	<< "', now()::abstime::int4)"
	<< ends;

#ifdef LOG_SQL
	elog	<< "sqlUser::writeEvent> "
		<< theLog.str()
		<< endl;
#endif

SQLDb->ExecCommandOk(theLog.str());

delete[] theLog.str();

}

const string sqlUser::getLastEvent(unsigned short eventType, unsigned int& eventTime)
{
strstream queryString;

queryString	<< "SELECT message,ts"
			<< " FROM userlog WHERE user_id = "
			<< id
			<< " AND event = "
			<< eventType
			<< " ORDER BY ts DESC LIMIT 1"
			<< ends;

#ifdef LOG_SQL
	elog	<< "sqlUser::getLastEvent> "
			<< queryString.str()
			<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_TUPLES_OK == status )
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


} // namespace gnuworld.
