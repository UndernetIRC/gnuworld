/* 
 * sqlUser.cc
 * 
 * Storage class for accessing user information either from the backend
 * or internal storage.
 * 
 * $Id: sqlUser.cc,v 1.2 2000/12/21 22:20:57 gte Exp $
 */
 
#include	<strstream>
#include	<string> 
#include	<cstring> 
#include	"ELog.h"
#include	"misc.h"
#include	"sqlUser.h" 

using std::string ; 
using std::endl ; 

namespace gnuworld
{

using namespace gnuworld ;

 
sqlUser::sqlUser(PgDatabase* _SQLDb)
 : id( 0 ),
   last_seen( 0 ),
   language_id( 0 ),
   flags( 0 ),
   last_update( ::time( 0 ) ),
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

	ExecStatusType status;
	elog << "sqlUser> Attempting to load data for user-id: " << userID << endl;
	
	strstream queryString;
	queryString << "SELECT id,user_name,password,last_seen,email,url,language_id,public_key,flags,last_update_by,last_update FROM users WHERE id = " << userID; 

	elog << "sqlQuery> " << queryString.str() << endl;

	if ((status = SQLDb->Exec(queryString.str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If the user doesn't exist, we won't get any rows back.
		 */ 

		if(SQLDb->Tuples() < 1) { 
			return (false);
		} 
		setAllMembers(); 
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

	ExecStatusType status;
	elog << "sqlUser> Attempting to load data for user-name: " << userName << endl;
	
	strstream queryString;
	queryString << "SELECT id,user_name,password,last_seen,email,url,language_id,public_key,flags,last_update_by,last_update FROM users WHERE lower(user_name) = '" + string_lower(userName) + "'";

	elog << "sqlQuery> " << queryString.str() << endl;

	if ((status = SQLDb->Exec(queryString.str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If the user doesn't exist, we won't get any rows back.
		 */ 

		if(SQLDb->Tuples() < 1) { 
			return (false);
		} 
		setAllMembers(); 
		return (true);
	} 

	return (false); 
} 


void sqlUser::setAllMembers()
{
	/*
	 *  Support function for both loadData's.
	 *  Assumes SQLDb contains a valid results set for all user information.
	 */

	id = atoi(SQLDb->GetValue(0, 0));
	user_name = SQLDb->GetValue(0, 1);
	password = SQLDb->GetValue(0, 2);
	last_seen = atoi(SQLDb->GetValue(0, 3));
	email = SQLDb->GetValue(0, 4);
	url = SQLDb->GetValue(0, 5);
	language_id = atoi(SQLDb->GetValue(0, 6));
	public_key = SQLDb->GetValue(0, 7); 
	flags = atoi(SQLDb->GetValue(0, 8));
	last_update_by = SQLDb->GetValue(0, 9); 
	last_update = atoi(SQLDb->GetValue(0, 10));
}


sqlUser::~sqlUser()
{}
 
} // Namespace gnuworld.
