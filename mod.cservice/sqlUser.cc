/* 
 * sqlUser.cc
 * 
 * Storage class for accessing user information either from the backend
 * or internal storage.
 * 
 * $Id: sqlUser.cc,v 1.13 2001/02/18 19:46:01 dan_karrels Exp $
 */
 
#include	<strstream>
#include	<string> 
#include	<cstring> 
#include	"ELog.h"
#include	"misc.h"
#include	"sqlUser.h" 
#include	"constants.h"

using std::string ; 
using std::endl ; 

namespace gnuworld
{
 
sqlUser::sqlUser(PgDatabase* _SQLDb)
 : networkClient(0),
   id( 0 ),
   user_name(),
   password(),
   last_seen( 0 ),
   email(),
   url(),
   language_id( 0 ),
   public_key(),
   flags( 0 ), 
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

elog	<< "sqlUser::loadData> Attempting to load data for user-id: "
	<< userID
	<< endl;

strstream queryString;
queryString	<< "SELECT "
		<< sql::user_fields 
		<< " FROM users,users_lastseen WHERE users.id ="
		<< "users_lastseen.user_id AND id = "
		<< userID
		<< ends;

elog	<< "sqlUser::loadData> "
	<< queryString.str()
	<< endl;

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
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
	delete[] queryString.str() ;

	return (true);
	} 

delete[] queryString.str() ;
return (false); 
} 

bool sqlUser::loadData(const string& userName)
{
/*
 *  With the open database handle 'SQLDb', retrieve information about
 *  'userID' and fill our member variables.
 */ 

elog	<< "sqlUser::loadData> Attempting to load data for user-name: "
	<< userName
	<< endl;
	
strstream queryString;
queryString	<< "SELECT "
		<< sql::user_fields 
		<< " FROM users,users_lastseen WHERE users.id ="
		<< " users_lastseen.user_id AND lower(user_name) = '"
		<< string_lower(userName) 
		<< "'"
		<< ends;

elog	<< "sqlUser::loadData> "
	<< queryString.str()
	<< endl;

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
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
	delete[] queryString.str() ;

	return (true);
	} 

delete[] queryString.str() ;
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
email = SQLDb->GetValue(row, 3);
url = SQLDb->GetValue(row, 4);
language_id = atoi(SQLDb->GetValue(row, 5));
public_key = SQLDb->GetValue(row, 6); 
flags = atoi(SQLDb->GetValue(row, 7));
last_updated_by = SQLDb->GetValue(row, 8); 
last_updated = atoi(SQLDb->GetValue(row, 9));
last_seen = atoi(SQLDb->GetValue(row, 10));

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

elog	<< "sqlUser::commit> "
	<< queryString.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
if( PGRES_COMMAND_OK != status )
	{
	// TODO: Log to msgchan here.
	elog	<< "sqlUser::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;

	delete[] queryString.str() ;
	return false;
 	} 

delete[] queryString.str() ;
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
		<< "last_updated = now()::abstime::int4 "
		<< queryCondition
		<< id
		<< ends;

elog	<< "sqlUser::commitLastSeen> "
	<< queryString.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
if( PGRES_COMMAND_OK != status )
	{
	// TODO: Log to msgchan here.
	elog	<< "sqlUser::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;

	delete[] queryString.str() ;
	return false;
 	} 

delete[] queryString.str() ;
return true;
}	
 
sqlUser::~sqlUser()
{
// No heap space allocated
}
 
} // Namespace gnuworld.
