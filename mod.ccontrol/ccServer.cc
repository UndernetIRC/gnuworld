/* 
 * ccServer.cc
 * 
 * Server class
 * 
 * $Id: ccServer.cc,v 1.9 2002/05/23 17:43:11 dan_karrels Exp $
 */
 
#include	<sstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"libpq++.h"
#include	"ELog.h"
#include	"misc.h"
#include	"ccServer.h" 
#include	"ccontrol.h"
#include	"Constants.h"

const char ccServer_h_rcsId[] = __CCSERVER_H ;
const char ccServer_cc_rcsId[] = "$Id: ccServer.cc,v 1.9 2002/05/23 17:43:11 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::stringstream ;
using std::ends ;

namespace uworld
{

unsigned int ccServer::numAllocated = 0;

ccServer::ccServer(PgDatabase* _SQLDb)
 : Name(),
   Uplink(),
   Numeric(),
   LastConnected( 0 ),
   LastSplitted( 0 ),
   SplitReason( "" ),
   Version( "Unknown" ),
   AddedOn( ::time(0) ),
   LastUpdated( 0 ),
   NetServer(NULL),
   SQLDb( _SQLDb )
{
++numAllocated;
}

ccServer::~ccServer()
{
--numAllocated;
}

bool ccServer::Insert()
{
static const char *Main = "INSERT into servers (Name,LastUplink,LastNumeric,LastConnected,SplitedOn,SplitReason,Version,AddedOn,LastUpdated) VALUES ('";

if(!dbConnected)
	{
	return false;
	}
stringstream theQuery;
theQuery	<< Main
		<< Name <<"','"
		<< Uplink << "','"
		<< Numeric << "',"
		<< LastConnected << ","
		<< LastSplitted 
		<< ",'" << SplitReason 
		<< "','" << Version
		<< "'," << AddedOn
		<< "," << LastUpdated
		<< ")" << ends;

elog	<< "ccontrol::Server::Insert::sqlQuery> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccontrol::Server::Insert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}

}

bool ccServer::Update()
{
static const char *Main = "UPDATE servers SET Name = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< Name
		<< "', LastUplink = '"
		<< Uplink
		<< "', LastNumeric = '"
		<< Numeric
		<< "', LastConnected = "
		<< LastConnected
		<< ",SplitedOn = "
		<< LastSplitted
		<< ", SplitReason = '"
		<< SplitReason 
		<< "', Version = ' "
		<< Version
		<< "', AddedOn = " 
		<< AddedOn
		<< ", LastUpdated = " 
		<< LastUpdated
		<< " WHERE lower(Name) = '" << string_lower(Name)
		<<  "'" << ends;

elog	<< "ccontrol::Server::Update> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccontrol::Server::Update> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
}

bool ccServer::loadData(string ServerName)
{
//static const char *Main = "SELECT name,lastuplink,lastconnected,splitedon,lastnumeric,splitreason FROM servers WHERE 
//lower(Name) = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< server::Query
		<< "Where lower(Name) = '"
		<< string_lower(ServerName)
		<< "'" << ends;

elog	<< "ccontrol::Server::LoadData> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::Server> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}

return loadDataFromDB();

}

bool ccServer::loadNumericData(string ServNumeric)
{
/*static const char *Main = "SELECT name,lastuplink,lastconnected,splitedon,lastnumeric,SplitReason FROM servers WHERE 
LastNumeric = '";*/

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< server::Query
		<< "Where LastNumeric = '"
		<< ServNumeric
		<< "'" << ends;

elog	<< "ccontrol::Server::LoadNumericData> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::Server> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}
return loadDataFromDB();

}

bool ccServer::loadDataFromDB(int place)
{

if(SQLDb->Tuples() == 0 )
    return false;
Name = SQLDb->GetValue(place,0);
Uplink = SQLDb->GetValue(0,1);
LastConnected = static_cast< time_t >( atoi( SQLDb->GetValue(place,2) ) ) ;
LastSplitted = static_cast< time_t >( atoi( SQLDb->GetValue(place,3) ) ) ;
Numeric = SQLDb->GetValue(place,4);
SplitReason = SQLDb->GetValue(place,5);
Version = SQLDb->GetValue(place,6);
AddedOn = static_cast< time_t >( atoi( SQLDb->GetValue(place,7) ) ) ;
LastUpdated = static_cast< time_t >( atoi( SQLDb->GetValue(place,8) ) ) ;
return true;

}

bool ccServer::Delete()
{
static const char *Main = "DELETE FROM servers WHERE lower(Name) = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< string_lower(Name)
		<< "'" << ends;

elog	<< "ccontrol::Server::Delete> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_COMMAND_OK != status ) 
	{
	elog	<< "ccontrol::Server::Delete> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
return true;
}

}

} //Namespace Gnuworld
