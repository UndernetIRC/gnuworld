/**
 * sqlLevel.cc
 *
 * Storage class for accessing channel user/level information either
 * from the backend or internal storage.
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
 * $Id: sqlLevel.cc,v 1.18 2009/06/09 15:40:29 mrbean_ Exp $
 */

#include	<sstream>
#include	<string>
#include	<iostream>
#include	<cstring>
#include	<ctime>
#include	"misc.h"
#include	"sqlLevel.h"
#include	"sqlUser.h"
#include	"sqlChannel.h"
#include	"constants.h"
#include	"cservice.h"
#include	"cservice_config.h"


namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

const sqlLevel::flagType sqlLevel::F_AUTOOP =	0x01 ;
const sqlLevel::flagType sqlLevel::F_PROTECT =	0x02 ;
const sqlLevel::flagType sqlLevel::F_FORCED =	0x04 ;
const sqlLevel::flagType sqlLevel::F_AUTOVOICE =0x08 ;
const sqlLevel::flagType sqlLevel::F_ONDB =		0x10 ;
const sqlLevel::flagType sqlLevel::F_AUTOINVITE = 0x20;

sqlLevel::sqlLevel(cservice* _bot)
 :channel_id(0),
 user_id(0),
 access(0),
 forced_access(0),
 flags(0),
 suspend_expires(0),
 suspend_level(0),
 suspend_by(),
 suspend_reason(),
 added(0),
 added_by(),
 last_modif(::time(NULL)),
 last_modif_by(),
 last_used(0),
 logger(_bot->getLogger()),
 SQLDb(_bot->SQLDb)
{
}

bool sqlLevel::loadData(unsigned int userID, unsigned int channelID)
{
/*
 * Fetch a matching Level record for this channel and user ID combo.
 */

LOG( TRACE, "Attempting to load level data for channel-id: {} and user-id: {}", channelID, userID) ;

stringstream queryString;
queryString	<< "SELECT "
		<< sql::level_fields
		<< " FROM levels WHERE channel_id = "
		<< channelID
		<< " AND user_id = "
		<< userID
		<< ends;

if( SQLDb->Exec(queryString, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	/*
	 *  If this combo doesn't exist, we won't get any rows back.
	 */

	if(SQLDb->Tuples() < 1)
		{
		return (false);
		}

	// Fetch dat from row 0
	setAllMembers(0);

	return (true);
	}

return (false);
}


void sqlLevel::setAllMembers(int row)
{
/*
 *  Support function for both loadData's.
 *  Assumes SQLDb contains a valid results set for all Level information.
 */

channel_id = atoi(SQLDb->GetValue(row, 0));
user_id = atoi(SQLDb->GetValue(row, 1));
access = atoi(SQLDb->GetValue(row, 2));
flags = atoi(SQLDb->GetValue(row, 3));
suspend_expires = atoi(SQLDb->GetValue(row, 4));
suspend_level = atoi(SQLDb->GetValue(row, 5));
suspend_by = SQLDb->GetValue(row, 6);
added = atoi(SQLDb->GetValue(row, 7));
added_by = SQLDb->GetValue(row, 8);
last_modif = atoi(SQLDb->GetValue(row, 9));
last_modif_by = SQLDb->GetValue(row, 10);
last_updated = atoi(SQLDb->GetValue(row, 11));
suspend_reason = SQLDb->GetValue(row, 12);
}

bool sqlLevel::commit()
{
/*
 *  Build an SQL statement to commit the transient data in this
 *  storage class back into the database.
 */

static const char* queryHeader =    "UPDATE levels ";

stringstream queryString;
queryString	<< queryHeader
		<< "SET flags = " << flags << ", "
		<< "access = " << access << ", "
		<< "suspend_expires = " << suspend_expires << ", "
		<< "suspend_level = " << suspend_level << ", "
		<< "suspend_by = '" << escapeSQLChars(suspend_by) << "', "
		<< "suspend_reason = '" << escapeSQLChars(suspend_reason) << "', "
		<< "added = " << added << ", "
		<< "added_by = '" << escapeSQLChars(added_by) << "', "
		<< "last_modif = " << last_modif << ", "
		<< "last_modif_by = '" << escapeSQLChars(last_modif_by) << "', "
		<< "last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int "
		<< " WHERE channel_id = " << channel_id
		<< " AND user_id = " << user_id
		<< ends;

if( !SQLDb->Exec(queryString ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false;
 	}

return true;
}

bool sqlLevel::insertRecord()
{
static const char* queryHeader = "INSERT INTO levels (channel_id,user_id,access,flags,added,added_by,last_modif,last_modif_by,last_updated) VALUES (";

stringstream queryString;
queryString	<< queryHeader
			<< channel_id << ", "
			<< user_id << ", "
			<< access << ", "
			<< flags << ", "
			<< added << ", '"
			<< escapeSQLChars(added_by) << "', "
			<< last_modif << ", '"
			<< escapeSQLChars(last_modif_by) << "', "
			<< "date_part('epoch', CURRENT_TIMESTAMP)::int)"
			<< ends;

if( !SQLDb->Exec(queryString ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false;
 	}

return true;
}

sqlLevel::~sqlLevel()
{
}

} // Namespace gnuworld.
