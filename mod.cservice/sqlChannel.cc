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
 * $Id: sqlChannel.cc,v 1.30 2001/09/09 23:29:13 gte Exp $
 */

#include	<strstream>
#include	<string>

#include	<cstring>

#include	"ELog.h"
#include	"misc.h"
#include	"sqlChannel.h"
#include	"constants.h"
#include	"cservice.h"
#include	"cservice_config.h"

const char sqlChannel_h_rcsId[] = __SQLCHANNEL_H ;
const char sqlChannel_cc_rcsId[] = "$Id: sqlChannel.cc,v 1.30 2001/09/09 23:29:13 gte Exp $" ;

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::strstream ;
using std::ends ;

const sqlChannel::flagType sqlChannel::F_NOPURGE  = 0x00000001 ;
const sqlChannel::flagType sqlChannel::F_SPECIAL  = 0x00000002 ;
const sqlChannel::flagType sqlChannel::F_NOREG    = 0x00000004 ;
const sqlChannel::flagType sqlChannel::F_NEVREG   = 0x00000008 ;
const sqlChannel::flagType sqlChannel::F_SUSPEND  = 0x00000010 ;
const sqlChannel::flagType sqlChannel::F_TEMP     = 0x00000020 ;
const sqlChannel::flagType sqlChannel::F_CAUTION  = 0x00000040 ;
const sqlChannel::flagType sqlChannel::F_VACATION = 0x00000080 ;
const sqlChannel::flagType sqlChannel::F_LOCKED   = 0x00000100 ;

const sqlChannel::flagType sqlChannel::F_ALWAYSOP  = 0x00010000 ;
const sqlChannel::flagType sqlChannel::F_STRICTOP  = 0x00020000 ;
const sqlChannel::flagType sqlChannel::F_NOOP      = 0x00040000 ;
const sqlChannel::flagType sqlChannel::F_AUTOTOPIC = 0x00080000 ;
const sqlChannel::flagType sqlChannel::F_OPONLY    = 0x00100000 ; // Deprecated
const sqlChannel::flagType sqlChannel::F_AUTOJOIN  = 0x00200000 ;

const unsigned int sqlChannel::EV_MISC     = 1 ;
const unsigned int sqlChannel::EV_JOIN     = 2 ;
const unsigned int sqlChannel::EV_PART     = 3 ;
const unsigned int sqlChannel::EV_OPERJOIN = 4 ;
const unsigned int sqlChannel::EV_OPERPART = 5 ;
const unsigned int sqlChannel::EV_FORCE    = 6 ;
const unsigned int sqlChannel::EV_REGISTER = 7 ;
const unsigned int sqlChannel::EV_PURGE    = 8 ;

/* Manually added Comment */
const unsigned int sqlChannel::EV_COMMENT 	= 9  ;
const unsigned int sqlChannel::EV_REMOVEALL	= 10 ;
const unsigned int sqlChannel::EV_IDLE		= 11 ;

sqlChannel::sqlChannel(PgDatabase* _SQLDb)
 : id(0),
   name(),
   flags(0),
   mass_deop_pro(3),
   flood_pro(7),
   url(),
   description(),
   comment(),
   keywords(),
   registered_ts(0),
   channel_ts(0),
   channel_mode(),
   userflags(0),
   last_topic(0),
   inChan(false),
   last_used(0),
   SQLDb( _SQLDb )
{
}


bool sqlChannel::loadData(const string& channelName)
{
/*
 *  With the open database handle 'SQLDb', retrieve information about
 *  'channelName' and fill our member variables.
 */

#ifdef LOG_DEBUG
	elog	<< "sqlChannel::loadData> Attempting to load data for"
		<< " channel-name: "
		<< channelName
		<< endl;
#endif

strstream queryString ;
queryString	<< "SELECT "
		<< sql::channel_fields
		<< " FROM channels WHERE registered_ts <> 0"
		<< " AND lower(name) = '"
		<< escapeSQLChars(string_lower(channelName))
		<< "'"
		<< ends ;

#ifdef LOG_SQL
	elog	<< "sqlChannel::loadData> "
		<< queryString.str()
		<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_TUPLES_OK == status )
	{
	/*
	 *  If the channel doesn't exist, we won't get any rows back.
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

bool sqlChannel::loadData(int channelID)
{
/*
 *  With the open database handle 'SQLDb', retrieve information about
 *  'channelID' and fill our member variables.
 */

#ifdef LOG_DEBUG
	elog	<< "sqlChannel::loadData> Attempting to load data for "
		<< "channel-id: "
		<< channelID
		<< endl;
#endif

strstream queryString;
queryString	<< "SELECT "
		<< sql::channel_fields
		<< " FROM channels WHERE registered_ts <> 0 AND id = "
		<< channelID
		<< ends ;

#ifdef LOG_SQL
	elog	<< "sqlChannel::loadData> "
		<< queryString.str()
		<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_TUPLES_OK == status )
	{
	/*
	 *  If the channel doesn't exist, we won't get any rows back.
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
comment = SQLDb->GetValue(row,7);
keywords = SQLDb->GetValue(row,8);
registered_ts = atoi(SQLDb->GetValue(row,9));
channel_ts = atoi(SQLDb->GetValue(row,10));
channel_mode = SQLDb->GetValue(row,11);
userflags = atoi(SQLDb->GetValue(row,12));
last_updated = atoi(SQLDb->GetValue(row,13));
}

bool sqlChannel::commit()
{
/*
 *  Build an SQL statement to commit the transient data in this storage class
 *  back into the database.
 */

static const char* queryHeader =    "UPDATE channels ";
static const char* queryCondition = "WHERE id = ";

strstream queryString;
queryString	<< queryHeader
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
		<< "description = '" << escapeSQLChars(description) << "', "
		<< "comment = '" << escapeSQLChars(comment) << "' "
		<< queryCondition << id
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlChannel::commit> "
		<< queryString.str()
		<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_COMMAND_OK != status )
	{
	elog	<< "sqlChannel::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;
	return false;
 	}

return true;
}

bool sqlChannel::insertRecord()
{
static const char* queryHeader = "INSERT INTO channels (name, flags, registered_ts, channel_ts, channel_mode, last_updated) VALUES (";

strstream queryString;
queryString	<< queryHeader
			<< "'" << escapeSQLChars(name) << "', "
			<< flags << ", "
			<< registered_ts << ", "
			<< channel_ts << ", '"
			<< escapeSQLChars(channel_mode) << "', "
			<< "now()::abstime::int4)"
			<< ends;

#ifdef LOG_SQL
	elog	<< "sqlChannel::insertRecord> "
			<< queryString.str()
			<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_COMMAND_OK != status )
	{
	// TODO: Log to msgchan here.
	elog	<< "sqlChannel::commit> Something went wrong: "
			<< SQLDb->ErrorMessage()
			<< endl;

	return false ;
 	}

return true;
}

sqlChannel::~sqlChannel()
{
	/* TODO: Clean up bans */
}

} // Namespace gnuworld
