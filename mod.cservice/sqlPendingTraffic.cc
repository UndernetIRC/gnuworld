/* 
 * sqlPendingTraffic.cc
 * 
 * $Id: sqlPendingTraffic.cc,v 1.3 2002/05/23 17:43:14 dan_karrels Exp $
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
const char sqlPendingTraffic_cc_rcsId[] = "$Id: sqlPendingTraffic.cc,v 1.3 2002/05/23 17:43:14 dan_karrels Exp $" ;

namespace gnuworld
{
using std::string ; 
using std::endl ; 
using std::ends ;
using std::stringstream ;

sqlPendingTraffic::sqlPendingTraffic(PgDatabase* _SQLDb)
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

ExecStatusType status = SQLDb->Exec(queryString.str().c_str()) ;

if( PGRES_COMMAND_OK != status )
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
	
	ExecStatusType status = SQLDb->Exec(queryString.str().c_str()) ;
	
	if( PGRES_COMMAND_OK != status )
		{
			elog << "sqlPendingTraffic::commit> Error updating pending_traffic "
				 << "record for " << ip_number << endl;
		}

	return true;
}

 
}

