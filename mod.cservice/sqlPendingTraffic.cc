/* 
 * sqlPendingTraffic.cc
 * 
 * $Id: sqlPendingTraffic.cc,v 1.1 2001/06/10 01:03:08 gte Exp $
 */
 
#include	<strstream>
#include	<string> 

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
const char sqlPendingTraffic_cc_rcsId[] = "$Id: sqlPendingTraffic.cc,v 1.1 2001/06/10 01:03:08 gte Exp $" ;

namespace gnuworld
{
using std::string ; 
using std::endl ; 

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
 
strstream queryString;
queryString << "INSERT INTO pending_traffic (channel_id, ip_number, join_count) VALUES ("
			<< channel_id << ", "
			<< theip_number << ", "
			<< join_count << ")"
			<< ends;

#ifdef LOG_SQL
	elog	<< "sqlPendingTraffic::insertRecord> "
		<< queryString.str()
		<< endl; 
#endif

ExecStatusType status = SQLDb->Exec(queryString.str()) ;
delete[] queryString.str() ;

if( PGRES_COMMAND_OK != status )
	{ 
	elog	<< "sqlPendingTraffic::commit> Something went wrong: "
			<< SQLDb->ErrorMessage()
			<< endl;

	return false;
 	} 

	return true;
}
 
}

