/* 
 * sqlPendingChannel.cc
 * 
 * Class which contains details about channels which are 'pending'
 * registration.
 * 
 * $Id: sqlPendingChannel.cc,v 1.8 2002/05/23 17:43:14 dan_karrels Exp $
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
const char sqlPendingChannel_cc_rcsId[] = "$Id: sqlPendingChannel.cc,v 1.8 2002/05/23 17:43:14 dan_karrels Exp $" ;

namespace gnuworld
{
using std::string ; 
using std::endl ; 
using std::ends ;
using std::stringstream ;

sqlPendingChannel::sqlPendingChannel(PgDatabase* _SQLDb)
:channel_id(0), 
join_count(0),
unique_join_count(0),
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

ExecStatusType status = SQLDb->Exec(theQuery.str().c_str()) ;

if( PGRES_TUPLES_OK == status )
	{
	for (int i = 0 ; i < SQLDb->Tuples(); i++)
		{ 
			unsigned int theIp = atoi(SQLDb->GetValue(i, 0));
//			elog << "IP: " << theIp << endl;

			sqlPendingTraffic* trafRecord = new sqlPendingTraffic(SQLDb);
			trafRecord->ip_number = theIp;
			trafRecord->join_count = atoi(SQLDb->GetValue(i, 1));
			trafRecord->channel_id = channel_id; 

			trafficList.insert(trafficListType::value_type(theIp, trafRecord));

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
			<< "unique_join_count = " << unique_join_count
			<< " WHERE channel_id = " 
			<< channel_id
			<< ends;

#ifdef LOG_SQL
	elog	<< "sqlPendingChannel::commit> "
			<< queryString.str().c_str()
			<< endl;
#endif

ExecStatusType status = SQLDb->Exec(queryString.str().c_str()) ;

if( PGRES_COMMAND_OK != status )
	{

	elog	<< "sqlPendingChannel::commit> Something went wrong: "
		<< SQLDb->ErrorMessage()
		<< endl;

	return false ;
 	} 

	/*
	 *  Next, iterate over the supporters and commit those details.
	 */

	for(supporterListType::iterator ptr = supporterList.begin();
		ptr !=  supporterList.end(); ++ptr)
		{ 
 }
 
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
	
	ExecStatusType status = SQLDb->Exec(queryString.str().c_str()) ;
	
	if( PGRES_COMMAND_OK != status )
		{
			elog << "sqlPendingChannel::commit> Error updating supporter "
				 << "record for " << sup_id << endl;
		}

	return true;
}

}
