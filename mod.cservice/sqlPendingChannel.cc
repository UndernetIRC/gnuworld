/**
 * sqlPendingChannel.cc
 * 
 * Class which contains details about channels which are 'pending'
 * registration.
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
 * $Id: sqlPendingChannel.cc,v 1.10 2007/08/28 16:10:12 dan_karrels Exp $
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
#include	"sqlPendingChannel.h"
#include	"sqlPendingTraffic.h"
 
const char sqlPendingChannel_h_rcsId[] = __SQLPENDINGCHANNEL_H ;
const char sqlPendingChannel_cc_rcsId[] = "$Id: sqlPendingChannel.cc,v 1.10 2007/08/28 16:10:12 dan_karrels Exp $" ;

namespace gnuworld
{
using std::string ; 
using std::endl ; 
using std::ends ;
using std::stringstream ;

sqlPendingChannel::sqlPendingChannel(dbHandle* _SQLDb)
:channel_id(0), 
join_count(0),
unique_join_count(0),
initialised(false),
SQLDb(_SQLDb)
{ 
}

/*
 *  Destructor to clean up any memory this class may have
 *  allocated.
 */

sqlPendingChannel::~sqlPendingChannel()
{
	for(trafficListType::iterator ptr = trafficList.begin();
		ptr !=  trafficList.end(); ++ptr)
		{ 
			sqlPendingTraffic* toDie = ptr->second;
//			elog << "Autocleanup of Traffic record for #" << toDie->ip_number << endl;
			delete(toDie);
		}

	for(trafficListType::iterator ptr = uniqueSupporterList.begin();
		ptr !=  uniqueSupporterList.end(); ++ptr)
		{
			sqlPendingTraffic* toDie = ptr->second;
//			elog << "Autocleanup of Traffic record for #" << toDie->ip_number << endl;
			delete(toDie);
		}
}	

void sqlPendingChannel::loadTrafficCache()
{
	/*
	 *  Load all associated Traffic records for this channel.
	 */ 
stringstream theQuery;
theQuery 	<< "SELECT ip_number, join_count FROM pending_traffic"
			<< " WHERE channel_id = " << channel_id
			<< ends;

#ifdef LOG_SQL
	elog	<< "sqlPendingChannel::loadTrafficCache> "
		<< theQuery.str().c_str()
		<< endl; 
#endif

if( SQLDb->Exec(theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{ 
			string theIp = SQLDb->GetValue(i, 0);
//			elog << "IP: " << theIp << endl;

			sqlPendingTraffic* trafRecord = new sqlPendingTraffic(SQLDb);
			trafRecord->ip_number = theIp;
			trafRecord->join_count = atoi(SQLDb->GetValue(i, 1));
			trafRecord->channel_id = channel_id; 

			trafficList.insert(trafficListType::value_type(theIp, trafRecord));
		}
	}
}

void sqlPendingChannel::loadSupportersTraffic()
{
	stringstream theQuery;
	theQuery 	<< "SELECT user_id,join_count FROM supporters"
				<< " WHERE channel_id = " << channel_id
				<< ends;

	#ifdef LOG_SQL
		elog	<< "sqlPendingChannel::loadUniqueTrafficCache> "
			<< theQuery.str().c_str()
			<< endl;
	#endif

	if( SQLDb->Exec(theQuery, true ) )
	//if( PGRES_TUPLES_OK == status )
	{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			string userId = SQLDb->GetValue(i, 0);
			sqlPendingTraffic* trafRecord = new sqlPendingTraffic(SQLDb);
			trafRecord->ip_number = userId;
			trafRecord->join_count = atoi(SQLDb->GetValue(i, 1));
			trafRecord->channel_id = channel_id;

			uniqueSupporterList.insert(trafficListType::value_type(userId, trafRecord));
		}
	}
}

bool sqlPendingChannel::commit()
{
/*
 * This is a 3 step commit.
 * 1. Update pending record values with new join_count
 * and unique_join_count.
 * 2. Update supporters records with new supporter count.
 * 3. Update Traffic table with new traffic counts.
 */

//elog << "Commiting Pending Channel Details: " << endl
//	<< "Channel ID: " << channel_id << endl
//	<< "Total Join Count: " << join_count
//	<< endl; 

/*
 *  Set the number of unique joins to be the number
 *  of elements in the IP traffic list.
 */

unique_join_count = trafficList.size();
 
stringstream queryString; 
queryString << "UPDATE pending SET "
			<< "join_count = " << join_count << ", "
			<< "unique_join_count = " << unique_join_count << ", "
			<< "first_init = '" << ((initialised == true) ? 'Y' : 'N') << "'"
			<< " WHERE channel_id = " 
			<< channel_id
			<< ends;

#ifdef LOG_SQL
	elog	<< "sqlPendingChannel::commit> "
			<< queryString.str().c_str()
			<< endl;
#endif

if( !SQLDb->Exec(queryString ) )
//if( PGRES_COMMAND_OK != status )
	{

	elog	<< "sqlPendingChannel::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;

	return false ;
 	} 

	/*
	 *  Next, iterate over the supporters and commit those details.
	 */

	//for(supporterListType::iterator ptr = supporterList.begin();
	//	ptr !=  supporterList.end(); ++ptr)
	//	{
	//	}
 
	return true;
}


bool sqlPendingChannel::commitSupporter(unsigned int sup_id, unsigned int count)
{
	stringstream queryString; 
	queryString << "UPDATE supporters SET "
				<< "join_count = " 
				<< count 
				<< " WHERE channel_id = "
				<< channel_id
				<< " AND user_id = "
				<< sup_id
				<< ends;
	
	#ifdef LOG_SQL
		elog	<< "sqlPendingChannel::commit> "
				<< queryString.str().c_str()
				<< endl;
	#endif
	
	if( !SQLDb->Exec(queryString ) )
//	if( PGRES_COMMAND_OK != status )
		{
			elog << "sqlPendingChannel::commit> Error updating supporter "
				 << "record for " << sup_id << endl;
		}

	return true;
}

}
