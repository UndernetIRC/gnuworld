/* 
 * sqlChannel.cc
 * 
 * Storage class for accessing user information either from the backend
 * or internal storage.
 *
 * 20/12/2000: Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 30/12/2000: Moved static SQL data to constants.h --Gte
 * Set loadData up to take data from rows other than 0.
 * 
 * $Id: sqlChannel.cc,v 1.20 2001/01/28 23:16:33 gte Exp $
 */
 
#include	<strstream>
#include	<string> 
#include	<cstring> 
#include	"ELog.h"
#include	"misc.h"
#include	"sqlChannel.h"
#include	"constants.h"
#include	"cservice.h"

using std::string ; 
using std::endl ; 
 
const char sqlChannel_h_rcsId[] = __SQLCHANNEL_H ;
const char sqlChannel_cc_rcsId[] = "$Id: sqlChannel.cc,v 1.20 2001/01/28 23:16:33 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld ;


sqlChannel::sqlChannel(PgDatabase* _SQLDb)
 :id(0),
 name(""),
 mass_deop_pro(3),
 flood_pro(7),
 url(""),
 description(""),
 keywords(""),
 registered_ts(0),
 channel_ts(0),
 channel_mode(""), 
 userflags(0),
 last_topic(0),
 inChan(false),
 SQLDb( _SQLDb )
{
}


bool sqlChannel::loadData(const string& channelName)
{
	/*
	 *  With the open database handle 'SQLDb', retrieve information about
	 *  'channelName' and fill our member variables.
	 */ 

	ExecStatusType status;
	elog << "sqlChannel::loadData> Attempting to load data for channel-name: " << channelName << endl;

    string queryString = "SELECT " + sql::channel_fields + " FROM channels WHERE registered_ts <> 0 AND deleted = 0 AND lower(name) = '" + string_lower(channelName) + "'";
	elog << "sqlChannel::loadData> " << queryString << endl;

	if ((status = SQLDb->Exec(queryString.c_str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If the channel doesn't exist, we won't get any rows back.
		 */ 

		if(SQLDb->Tuples() < 1) { 
			return (false);
		} 
		setAllMembers(0); 
		return (true);
	} 
	return (false); 
} 

bool sqlChannel::loadData(int channelID)
{
	/*
	 *  With the open database handle 'SQLDb', retrieve information about
	 *  'channelID' and fill our member variables.
	 */ 

	ExecStatusType status;
	elog << "sqlChannel::loadData> Attempting to load data for channel-id: " << channelID << endl;
	
	strstream queryString;
	queryString << "SELECT " + sql::channel_fields + " FROM channels WHERE registered_ts <> 0 AND id = " << channelID; 

	elog << "sqlChannel::loadData> " << queryString.str() << endl;

	if ((status = SQLDb->Exec(queryString.str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If the channel doesn't exist, we won't get any rows back.
		 */ 

		if(SQLDb->Tuples() < 1) { 
			return (false);
		} 
		setAllMembers(0); 
		return (true);
	} 

	return (false); 
} 


void sqlChannel::setAllMembers(int row)
{
	/*
	 *  Support function for both loadData's.
	 *  Assumes SQLDb contains a valid results set for all channel information.
	 */

	id = atoi(SQLDb->GetValue(row, 0));
	name = SQLDb->GetValue(row, 1);
	flags = atoi(SQLDb->GetValue(row, 2));
	mass_deop_pro = atoi(SQLDb->GetValue(row,3));
	flood_pro = atoi(SQLDb->GetValue(row,4));
	url = SQLDb->GetValue(row,5);
	description = SQLDb->GetValue(row,6);
	keywords = SQLDb->GetValue(row,7);
	registered_ts = atoi(SQLDb->GetValue(row,8));
	channel_ts = atoi(SQLDb->GetValue(row,9));
	channel_mode = SQLDb->GetValue(row,10); 
	userflags = atoi(SQLDb->GetValue(row,11));
	last_updated = atoi(SQLDb->GetValue(row,12));
}

bool sqlChannel::commit()
{
	/*
	 *  Build an SQL statement to commit the transient data in this storage class
	 *  back into the database.
	 */

	ExecStatusType status;
	static const char* queryHeader =    "UPDATE channels ";
	static const char* queryCondition = "WHERE id = "; 

	strstream queryString;
	queryString << queryHeader 
	<< "SET flags = " << flags << ", "
	<< "mass_deop_pro = " << mass_deop_pro << ", "
	<< "flood_pro = " << flood_pro << ", "
	<< "url = '" << escapeSQLChars(url) << "', "
	<< "keywords = '" << escapeSQLChars(keywords) << "', "
	<< "registered_ts = " << registered_ts << ", "
	<< "channel_ts = " << channel_ts << ", "
	<< "channel_mode = '" << channel_mode << "', " 
	<< "userflags = " << userflags << ", "
	<< "last_updated = now()::abstime::int4, "
	<< "description = '" << escapeSQLChars(description) << "' "
	<< queryCondition << id
	<< ends;

	elog << "sqlChannel::commit> " << queryString.str() << endl; 

	if ((status = SQLDb->Exec(queryString.str())) != PGRES_COMMAND_OK)
	{
		elog << "sqlChannel::commit> Something went wrong: " << SQLDb->ErrorMessage() << endl; // Log to msgchan here.
		return false;
 	} 

 	return true;
}	

sqlChannel::~sqlChannel()
{
}

} // Namespace gnuworld
