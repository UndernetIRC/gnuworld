/* 
 * sqlUser.cc
 * 
 * Storage class for accessing user information either from the backend
 * or internal storage.
 * 
 * $Id: sqlUser.cc,v 1.5 2001/01/02 07:55:12 gte Exp $
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
	elog << "sqlUser::loadData> Attempting to load data for user-id: " << userID << endl;
	
	strstream queryString;
	queryString << "SELECT " << sql::user_fields 
	<< " FROM users WHERE id = " << userID 
	<< ends;

	elog << "sqlUser::loadData> " << queryString.str() << endl;

	if ((status = SQLDb->Exec(queryString.str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If the user doesn't exist, we won't get any rows back.
		 */ 

		if(SQLDb->Tuples() < 1) { 
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

	ExecStatusType status;
	elog << "sqlUser::loadData> Attempting to load data for user-name: " << userName << endl;
	
	strstream queryString;
	queryString << "SELECT " << sql::user_fields 
	<< " FROM users WHERE lower(user_name) = '" << string_lower(userName) 
	<< "'" << ends;

	elog << "sqlUser::loadData> " << queryString.str() << endl;

	if ((status = SQLDb->Exec(queryString.str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If the user doesn't exist, we won't get any rows back.
		 */ 

		if(SQLDb->Tuples() < 1) { 
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
	last_seen = atoi(SQLDb->GetValue(row, 3));
	email = SQLDb->GetValue(row, 4);
	url = SQLDb->GetValue(row, 5);
	language_id = atoi(SQLDb->GetValue(row, 6));
	public_key = SQLDb->GetValue(row, 7); 
	flags = atoi(SQLDb->GetValue(row, 8));
	last_update_by = SQLDb->GetValue(row, 9); 
	last_update = atoi(SQLDb->GetValue(row, 10));
}


sqlUser::~sqlUser()
{}
 
} // Namespace gnuworld.
