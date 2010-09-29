/**
 * sqlPendingTraffic.cc
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
 * $Id: sqlPendingTraffic.cc,v 1.5 2007/08/28 16:10:12 dan_karrels Exp $
 */
 
#include	<sstream>
#include	<string> 
#include	<iostream>

#include	<cstring> 
#include	<ctime>

#include	"ELog.h"
#include	"misc.h"
#include	"sqlLevel.h"
#include	"sqlUser.h"
#include	"sqlChannel.h"
#include	"constants.h"
#include	"cservice_config.h"
#include	"sqlPendingTraffic.h"
 
const char sqlPendingTraffic_h_rcsId[] = __SQLPENDINGTRAFFIC_H ;
const char sqlPendingTraffic_cc_rcsId[] = "$Id: sqlPendingTraffic.cc,v 1.5 2007/08/28 16:10:12 dan_karrels Exp $" ;

namespace gnuworld
{
using std::string ; 
using std::endl ; 
using std::ends ;
using std::stringstream ;

sqlPendingTraffic::sqlPendingTraffic(dbHandle* _SQLDb)
:channel_id(0),
ip_number(0),
join_count(0),
SQLDb(_SQLDb)
{ 
}

bool sqlPendingTraffic::insertRecord()
{ 
int theip_number = ip_number;
 
stringstream queryString;
queryString << "INSERT INTO pending_traffic (channel_id, ip_number, join_count) VALUES ("
			<< channel_id << ", "
			<< theip_number << ", "
			<< join_count << ")"
			<< ends;

#ifdef LOG_SQL
	elog	<< "sqlPendingTraffic::insertRecord> "
		<< queryString.str().c_str()
		<< endl; 
#endif

if( !SQLDb->Exec(queryString ) )
//if( PGRES_COMMAND_OK != status )
	{ 
	elog	<< "sqlPendingTraffic::commit> Something went wrong: "
			<< SQLDb->ErrorMessage()
			<< endl;

	return false;
 	} 

	return true;
}

bool sqlPendingTraffic::commit()
{
	int theip_number = ip_number;
	
	stringstream queryString; 
	queryString << "UPDATE pending_traffic SET "
				<< "join_count = " 
				<< join_count
				<< " WHERE channel_id = "
				<< channel_id
				<< " AND ip_number = "
				<< theip_number
				<< ends;
	
	#ifdef LOG_SQL
		elog	<< "sqlPendingTraffic::commit> "
				<< queryString.str().c_str()
				<< endl;
	#endif
	
	if( !SQLDb->Exec(queryString ) )
//	if( PGRES_COMMAND_OK != status )
		{
			elog << "sqlPendingTraffic::commit> Error updating pending_traffic "
				 << "record for " << ip_number << endl;
		}

	return true;
}

 
}

