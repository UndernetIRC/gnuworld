/**
 * sqlBan.cc
 *
 * Storage class for accessing Ban information either from the backend
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
 * $Id: sqlBan.cc,v 1.11 2009/06/25 19:05:23 mrbean_ Exp $
 */

#include	<sstream>
#include	<string>
#include	<iostream>

#include	<cstring>

#include	"ELog.h"
#include	"misc.h"
#include	"sqlBan.h"
#include	"constants.h"
#include	"cservice.h"
#include	"cservice_config.h"
#include	"nickUserCidr.h"
#include	"HostBanMatcher.h"
#include	"CIDRBanMatcher.h"

const char sqlBan_h_rcsId[] = __SQLBAN_H ;
const char sqlBan_cc_rcsId[] = "$Id: sqlBan.cc,v 1.11 2009/06/25 19:05:23 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

sqlBan::sqlBan(dbHandle* _SQLDb)
  : id(0),
    channel_id(0),
    banmask(),
    set_by(),
    set_ts(0),
    level(0),
    expires(0),
    reason(),
    last_updated(0),
    SQLDb(_SQLDb)
{
	matcher = NULL;
	
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
initMatcher();
}

bool sqlBan::commit()
{
/*
 *  Build an SQL statement to commit the transient data in this storage class
 *  back into the database.
 */

static const char* queryHeader =    "UPDATE bans ";

stringstream queryString;
queryString	<< queryHeader
		<< "SET channel_id = " << channel_id << ", "
		<< "set_by = '" << escapeSQLChars(set_by) << "', "
		<< "set_ts = " << set_ts << ", "
		<< "level = " << level << ", "
		<< "expires = " << expires << ", "
		<< "banmask = '" << escapeSQLChars(banmask) << "', "
		<< "reason = '" << escapeSQLChars(reason) << "', "
		<< "last_updated = now()::abstime::int4 "
		<< " WHERE id = " << id
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlBan::commit> "
		<< queryString.str().c_str()
		<< endl;
#endif

if( !SQLDb->Exec(queryString ) )
//if( PGRES_COMMAND_OK != status )
	{
	// TODO: Log to msgchan here.
	elog	<< "sqlBan::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;

	return false ;
 	}

return true ;
}

bool sqlBan::insertRecord()
{
/*
 *  Build an SQL statement to insert this as a new record in the db.
 */

static const char* queryHeader = "INSERT INTO bans (channel_id,banmask,set_by,set_ts,level,expires,reason,last_updated) VALUES (";

stringstream queryString;
queryString	<< queryHeader
		<< channel_id << ", '"
		<< escapeSQLChars(banmask) << "', '"
		<< escapeSQLChars(set_by) << "', "
		<< set_ts << ", "
		<< level << ", "
		<< expires << ", '"
		<< escapeSQLChars(reason) << "', "
		<< "now()::abstime::int4); SELECT currval('bans_id_seq')"
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlBan::insertRecord> "
		<< queryString.str().c_str()
		<< endl;
#endif

if( !SQLDb->Exec(queryString, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	// TODO: Log to msgchan here.
	elog	<< "sqlBan::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;

	return false ;
 	}

id = atoi(SQLDb->GetValue(0,0));
return true ;
}

bool sqlBan::deleteRecord()
{
/*
 *  Build an SQL statement to delete this record from the db.
 */

static const char* queryHeader = "DELETE FROM bans WHERE id = ";

stringstream queryString;
queryString	<< queryHeader
		<< id
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlBan::delete> "
		<< queryString.str()
		<< endl;
#endif

if( !SQLDb->Exec(queryString ) )
//if( PGRES_COMMAND_OK != status )
	{
	// TODO: Log to msgchan here.
	elog	<< "sqlBan::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;

	return false ;
 	}

return true ;
}

void sqlBan::initMatcher()
{
	if(matcher)
		delete matcher;
	
	xNickUserCIDR cidr = xNickUserCIDR(banmask);
	if(cidr.GetValid())
	{
		matcher = new CIDRBanMatcher(banmask);
	} else {
		matcher = new HostBanMatcher(banmask);
	}
}

sqlBan::~sqlBan()
{
	if(matcher)
		delete matcher;
}



} // Namespace gnuworld.
