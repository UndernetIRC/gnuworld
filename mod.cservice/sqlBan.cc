/* 
 * sqlBan.cc
 * 
 * Storage class for accessing Ban information either from the backend
 * or internal storage.
 * 
 * $Id: sqlBan.cc,v 1.2 2001/01/25 00:19:13 gte Exp $
 */
 
#include	<strstream>
#include	<string> 
#include	<cstring> 
#include	"ELog.h"
#include	"misc.h"
#include	"sqlBan.h"
#include	"constants.h"
#include	"cservice.h"

using std::string ; 
using std::endl ; 
 
const char sqlBan_h_rcsId[] = __SQLBAN_H ;
const char sqlBan_cc_rcsId[] = "$Id: sqlBan.cc,v 1.2 2001/01/25 00:19:13 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld ;


sqlBan::sqlBan(PgDatabase* _SQLDb)
:id(0),
channel_id(0),
banmask(""),
set_by(""),
set_ts(0), 
level(0),
expires(0),
reason(""),
last_updated(0),
SQLDb(_SQLDb)
{ 
}
 
void sqlBan::setAllMembers(int row)
{
	/*
 	 *  Loads data from the Postgres backend.
	 *  Assumes SQLDb contains a valid results set for all Ban information.
	 */ 

	id = atoi(SQLDb->GetValue(row, 0));
	channel_id = atoi(SQLDb->GetValue(row, 1));
	banmask = SQLDb->GetValue(row, 2);
	set_by = SQLDb->GetValue(row, 3);
	set_ts = atoi(SQLDb->GetValue(row, 4));
	level = atoi(SQLDb->GetValue(row, 5));
	expires = atoi(SQLDb->GetValue(row, 6));
	reason = SQLDb->GetValue(row, 7); 
	last_updated = atoi(SQLDb->GetValue(row, 8)); 
}

bool sqlBan::commit()
{
	/*
	 *  Build an SQL statement to commit the transient data in this storage class
	 *  back into the database.
	 */

	ExecStatusType status;
	static const char* queryHeader =    "UPDATE bans ";
 
	strstream queryString;
	queryString << queryHeader 
	<< "SET channel_id = " << channel_id << ", "
	<< "set_by = '" << set_by << "', "
	<< "set_ts = " << set_ts << ", "
	<< "level = " << level << ", "
	<< "expires = " << expires << ", "
	<< "banmask = '" << banmask << "', "
	<< "reason = '" << reason << "', " 
	<< "last_updated = now()::abstime::int4 "
	<< " WHERE id = " << id
	<< ends;

	elog << "sqlBan::commit> " << queryString.str() << endl; 

	if ((status = SQLDb->Exec(queryString.str())) != PGRES_COMMAND_OK)
	{
		elog << "sqlBan::commit> Something went wrong: " << SQLDb->ErrorMessage() << endl; // Log to msgchan here.
		return false;
 	} 

 	return true;
}	

bool sqlBan::insertRecord()
{
	/*
	 *  Build an SQL statement to insert this as a new record in the db.
	 */

	ExecStatusType status;
	static const char* queryHeader = "INSERT INTO bans (channel_id,banmask,set_by,set_ts,level,expires,reason,last_updated) VALUES ("; 

	strstream queryString;
	queryString << queryHeader 
	<< channel_id << ", '"
	<< banmask << "', '"
	<< set_by << "', "
	<< set_ts << ", "
	<< level << ", "
	<< expires << ", '" 
	<< escapeSQLChars(reason) << "', " 
	<< "now()::abstime::int4); SELECT currval('bans_id_seq')" 
	<< ends;

	elog << "sqlBan::insertRecord> " << queryString.str() << endl; 
	if ((status = SQLDb->Exec(queryString.str())) != PGRES_TUPLES_OK) 
	{
		elog << "sqlBan::commit> Something went wrong: " << SQLDb->ErrorMessage() << endl; // Log to msgchan here.
		return false; 
 	} else
	{
		id = atoi(SQLDb->GetValue(0,0)); 
	}
 
 	return true;
} 

bool sqlBan::deleteRecord()
{
	/*
	 *  Build an SQL statement to delete this record from the db.
	 */

	ExecStatusType status;
	static const char* queryHeader = "DELETE FROM bans WHERE id = ";
 
	strstream queryString;
	queryString << queryHeader 
	<< id << ends;

	elog << "sqlBan::delete> " << queryString.str() << endl; 

	if ((status = SQLDb->Exec(queryString.str())) != PGRES_COMMAND_OK)
	{
		elog << "sqlBan::commit> Something went wrong: " << SQLDb->ErrorMessage() << endl; // Log to msgchan here.
		return false;
 	} 

 	return true;
}	


sqlBan::~sqlBan()
{
}

} // Namespace gnuworld.
