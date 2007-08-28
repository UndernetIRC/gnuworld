/**
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
 * $Id: sqlChannel.cc,v 1.43 2007/08/28 16:10:12 dan_karrels Exp $
 */

#include	<sstream>
#include	<string>
#include	<iostream>

#include	<cstring>

#include	"ELog.h"
#include	"misc.h"
#include	"sqlChannel.h"
#include	"constants.h"
#include	"cservice.h"
#include	"cservice_config.h"
#include	"dbHandle.h"

const char sqlChannel_h_rcsId[] = __SQLCHANNEL_H ;
const char sqlChannel_cc_rcsId[] = "$Id: sqlChannel.cc,v 1.43 2007/08/28 16:10:12 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::stringstream ;
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
const sqlChannel::flagType sqlChannel::F_FLOATLIM = 0x00000200 ;
const sqlChannel::flagType sqlChannel::F_MIA      = 0x00000400 ;

const sqlChannel::flagType sqlChannel::F_ALWAYSOP  = 0x00010000 ;
const sqlChannel::flagType sqlChannel::F_STRICTOP  = 0x00020000 ;
const sqlChannel::flagType sqlChannel::F_NOOP      = 0x00040000 ;
const sqlChannel::flagType sqlChannel::F_AUTOTOPIC = 0x00080000 ;
const sqlChannel::flagType sqlChannel::F_OPONLY    = 0x00100000 ; // Deprecated
const sqlChannel::flagType sqlChannel::F_AUTOJOIN  = 0x00200000 ;
const sqlChannel::flagType sqlChannel::F_NOFORCE   = 0x00400000 ; // Reserved for use by Planetarion.

const int sqlChannel::EV_MISC     = 1 ;
const int sqlChannel::EV_JOIN     = 2 ;
const int sqlChannel::EV_PART     = 3 ;
const int sqlChannel::EV_OPERJOIN = 4 ;
const int sqlChannel::EV_OPERPART = 5 ;
const int sqlChannel::EV_FORCE    = 6 ;
const int sqlChannel::EV_REGISTER = 7 ;
const int sqlChannel::EV_PURGE    = 8 ;

/* Manually added Comment */
const int sqlChannel::EV_COMMENT 	= 9  ;
const int sqlChannel::EV_REMOVEALL	= 10 ;
const int sqlChannel::EV_IDLE		= 11 ;

/* other events */
const int sqlChannel::EV_MGRCHANGE	= 12 ;
const int sqlChannel::EV_ADMREJECT	= 13 ;
const int sqlChannel::EV_WITHDRAW	= 14 ;
const int sqlChannel::EV_NEWAPP		= 15 ;
const int sqlChannel::EV_NONSUPPORT	= 16 ;
const int sqlChannel::EV_ADMREVIEW	= 17 ;
const int sqlChannel::EV_CLRREVIEW	= 18 ;
const int sqlChannel::EV_SUSPEND	= 19 ;
const int sqlChannel::EV_UNSUSPEND	= 20 ;

sqlChannel::sqlChannel(dbHandle* _SQLDb)
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
   limit_offset(3),
   limit_period(20),
   last_limit_check(0),
   limit_grace(2),
   limit_max(0),
   max_bans(0),
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

stringstream queryString ;
queryString	<< "SELECT "
		<< sql::channel_fields
		<< " FROM channels WHERE registered_ts <> 0"
		<< " AND lower(name) = '"
		<< escapeSQLChars(string_lower(channelName))
		<< "'"
		<< ends ;

#ifdef LOG_SQL
	elog	<< "sqlChannel::loadData> "
		<< queryString.str().c_str()
		<< endl;
#endif

if( SQLDb->Exec(queryString, true ) )
//if( PGRES_TUPLES_OK == status )
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

stringstream queryString;
queryString	<< "SELECT "
		<< sql::channel_fields
		<< " FROM channels WHERE registered_ts <> 0 AND id = "
		<< channelID
		<< ends ;

#ifdef LOG_SQL
	elog	<< "sqlChannel::loadData> "
		<< queryString.str().c_str()
		<< endl;
#endif

if( SQLDb->Exec(queryString, true ) )
//if( PGRES_TUPLES_OK == status )
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

id = atoi(SQLDb->GetValue(row, 0).c_str());
name = SQLDb->GetValue(row, 1);
flags = atoi(SQLDb->GetValue(row, 2).c_str());
mass_deop_pro = atoi(SQLDb->GetValue(row,3).c_str());
flood_pro = atoi(SQLDb->GetValue(row,4).c_str());
url = SQLDb->GetValue(row,5);
description = SQLDb->GetValue(row,6);
comment = SQLDb->GetValue(row,7);
keywords = SQLDb->GetValue(row,8);
registered_ts = atoi(SQLDb->GetValue(row,9).c_str());
channel_ts = atoi(SQLDb->GetValue(row,10).c_str());
channel_mode = SQLDb->GetValue(row,11);
userflags = atoi(SQLDb->GetValue(row,12));
last_updated = atoi(SQLDb->GetValue(row,13));
limit_offset = atoi(SQLDb->GetValue(row,14));
limit_period = atoi(SQLDb->GetValue(row,15));
limit_grace = atoi(SQLDb->GetValue(row,16));
limit_max = atoi(SQLDb->GetValue(row,17));
max_bans = atoi(SQLDb->GetValue(row,18));
}

bool sqlChannel::commit()
{
/*
 *  Build an SQL statement to commit the transient data in this storage class
 *  back into the database.
 */

static const char* queryHeader =    "UPDATE channels ";
static const char* queryCondition = "WHERE id = ";

stringstream queryString;
queryString	<< queryHeader
		<< "SET flags = " << flags << ", "
		<< "mass_deop_pro = " << mass_deop_pro << ", "
		<< "flood_pro = " << flood_pro << ", "
		<< "url = '" << escapeSQLChars(url) << "', "
		<< "keywords = '" << escapeSQLChars(keywords) << "', "
		<< "registered_ts = " << registered_ts << ", "
		<< "channel_ts = " << channel_ts << ", "
		<< "channel_mode = '" << escapeSQLChars(channel_mode) << "', "
		<< "userflags = " << userflags << ", "
		<< "last_updated = now()::abstime::int4, "
		<< "limit_offset = " << limit_offset << ", "
		<< "limit_period = " << limit_period << ", "
		<< "limit_grace = " << limit_grace << ", "
		<< "limit_max = " << limit_max << ", "
		<< "max_bans = " << max_bans << ", "
		<< "description = '" << escapeSQLChars(description) << "', "
		<< "comment = '" << escapeSQLChars(comment) << "' "
		<< queryCondition << id
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlChannel::commit> "
		<< queryString.str().c_str()
		<< endl;
#endif

if( !SQLDb->Exec(queryString ) )
//if( PGRES_COMMAND_OK != status )
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

stringstream queryString;
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
			<< queryString.str().c_str()
			<< endl;
#endif

if( !SQLDb->Exec(queryString ) )
//if( PGRES_COMMAND_OK != status )
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
