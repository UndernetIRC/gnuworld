/* 
 * sqlChannel.cc
 * 
 * Storage class for accessing user information either from the backend
 * or internal storage.
 * 
 * $Id: sqlChannel.cc,v 1.7 2000/12/30 00:32:37 gte Exp $
 */
 
#include	<strstream>
#include	<string> 
#include	<cstring> 
#include	"ELog.h"
#include	"misc.h"
#include	"sqlChannel.h" 

using std::string ; 
using std::endl ; 
 
const char sqlChannel_h_rcsId[] = __SQLCHANNEL_H ;
const char sqlChannel_cc_rcsId[] = "$Id: sqlChannel.cc,v 1.7 2000/12/30 00:32:37 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld ;


sqlChannel::sqlChannel(PgDatabase* _SQLDb)
{
	/*
	 *  Constructor, sets the database handle.
	 */
	SQLDb = _SQLDb;
}


bool sqlChannel::loadData(const string& channelName)
{
	/*
	 *  With the open database handle 'SQLDb', retrieve information about
	 *  'channelName' and fill our member variables.
	 */ 

	ExecStatusType status;
	elog << "sqlChannel> Attempting to load data for channel-name: " << channelName << endl;

    string queryString = "SELECT id,name,flags,mass_deop_pro,flood_pro,url,description,keywords,registered_ts,channel_ts,channel_mode,channel_key,channel_limit FROM channels WHERE lower(name) = '" + string_lower(channelName) + "'";
	elog << "sqlQuery> " << queryString << endl;

	if ((status = SQLDb->Exec(queryString.c_str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If the channel doesn't exist, we won't get any rows back.
		 */ 

		if(SQLDb->Tuples() < 1) { 
			return (false);
		} 
		setAllMembers(); 
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
	elog << "sqlChannel> Attempting to load data for channel-id: " << channelID << endl;
	
	strstream queryString;
	queryString << "SELECT id,name,flags,mass_deop_pro,flood_pro,url,description,keywords,registered_ts,channel_ts,channel_mode,channel_key,channel_limit FROM channels WHERE id = " << channelID; 

	elog << "sqlQuery> " << queryString.str() << endl;

	if ((status = SQLDb->Exec(queryString.str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If the channel doesn't exist, we won't get any rows back.
		 */ 

		if(SQLDb->Tuples() < 1) { 
			return (false);
		} 
		setAllMembers(); 
		return (true);
	} 

	return (false); 
} 


void sqlChannel::setAllMembers()
{
	/*
	 *  Support function for both loadData's.
	 *  Assumes SQLDb contains a valid results set for all channel information.
	 */

	id = atoi(SQLDb->GetValue(0, 0));
	name = SQLDb->GetValue(0, 1);
	flags = atoi(SQLDb->GetValue(0, 2));
	mass_deop_pro = atoi(SQLDb->GetValue(0,3));
	flood_pro = atoi(SQLDb->GetValue(0,4));
	url = SQLDb->GetValue(0,5);
	description = SQLDb->GetValue(0,6);
	keywords = SQLDb->GetValue(0,7);
	registered_ts = atoi(SQLDb->GetValue(0,8));
	channel_ts = atoi(SQLDb->GetValue(0,9));
	channel_mode = SQLDb->GetValue(0,10);
	channel_key = SQLDb->GetValue(0,11);
	channel_limit = atoi(SQLDb->GetValue(0,12)); 
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
	<< "SET name = '" << name << "', "
	<< "flags = " << flags << ", "
	<< "mass_deop_pro = " << mass_deop_pro << ", "
	<< "flood_pro = " << flood_pro << ", "
	<< "url = '" << url << "', "
	<< "keywords = '" << keywords << "', "
	<< "registered_ts = " << registered_ts << ", "
	<< "channel_ts = " << channel_ts << ", "
	<< "channel_mode = '" << channel_mode << "', "
	<< "channel_key = '" << channel_key << "', "
	<< "channel_limit = " << channel_limit << ", "
	<< "last_update = " << ::time(NULL) << ", "
	<< "description = '" << description << "' "
	<< queryCondition << id
	<< ends;

	elog << "sqlQuery> " << queryString.str() << endl; 

	if ((status = SQLDb->Exec(queryString.str())) != PGRES_COMMAND_OK)
	{
		elog << "sqlQuery> Something went wrong: " << SQLDb->ErrorMessage() << endl; // Log to msgchan here.
		return false;
 	} 

 	return true;
}	

sqlChannel::~sqlChannel()
{
}

} // Namespace gnuworld
