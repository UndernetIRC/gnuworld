/* 
 * ccServer.cc
 * 
 * Server class
 * 
 * $Id: ccServer.cc,v 1.7 2001/11/21 20:54:40 mrbean_ Exp $
 */
 
#include	<strstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"libpq++.h"
#include	"ELog.h"
#include	"misc.h"
#include	"ccServer.h" 
#include	"ccontrol.h"

const char ccServer_h_rcsId[] = __CCSERVER_H ;
const char ccServer_cc_rcsId[] = "$Id: ccServer.cc,v 1.7 2001/11/21 20:54:40 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::strstream ;
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
static const char *Main = "INSERT into servers (Name,LastUplink,LastNumeric,LastConnected,SplitedOn,SplitReason) VALUES ('";

if(!dbConnected)
	{
	return false;
	}
strstream theQuery;
theQuery	<< Main
		<< Name <<"','"
		<< Uplink << "','"
		<< Numeric << "',"
		<< LastConnected << ","
		<< LastSplitted 
		<< ",'" << SplitReason << "')"
		<< ends;

elog	<< "ccontrol::Server::Insert::sqlQuery> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

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

strstream theQuery;
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
		<< "' WHERE lower(Name) = '" << string_lower(Name)
		<<  "'" << ends;

elog	<< "ccontrol::Server::Update> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

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
static const char *Main = "SELECT name,lastuplink,lastconnected,splitedon,lastnumeric,splitreason FROM servers WHERE 
lower(Name) = '";

if(!dbConnected)
	{
	return false;
	}

strstream theQuery;
theQuery	<< Main
		<< string_lower(ServerName)
		<< "'" << ends;

elog	<< "ccontrol::Server::LoadData> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::Server> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}

if(SQLDb->Tuples() == 0 )
    return false;
Name = SQLDb->GetValue(0,0);
Uplink = SQLDb->GetValue(0,1);
LastConnected = static_cast< time_t >( atoi( SQLDb->GetValue(0,2) ) ) ;
LastSplitted = static_cast< time_t >( atoi( SQLDb->GetValue(0,3) ) ) ;
Numeric = SQLDb->GetValue(0,4);
SplitReason=SQLDb->GetValue(0,5);
return true;
}

bool ccServer::loadNumericData(string ServNumeric)
{
static const char *Main = "SELECT name,lastuplink,lastconnected,splitedon,lastnumeric,SplitReason FROM servers WHERE 
LastNumeric = '";

if(!dbConnected)
	{
	return false;
	}

strstream theQuery;
theQuery	<< Main
		<< ServNumeric
		<< "'" << ends;

elog	<< "ccontrol::Server::LoadNumericData> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::Server> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}

if(SQLDb->Tuples() == 0 )
    return false;
Name = SQLDb->GetValue(0,0);
Uplink = SQLDb->GetValue(0,1);
LastConnected = static_cast< time_t >( atoi( SQLDb->GetValue(0,2) ) ) ;
LastSplitted = static_cast< time_t >( atoi( SQLDb->GetValue(0,3) ) ) ;
Numeric = SQLDb->GetValue(0,4);
SplitReason = SQLDb->GetValue(0,5);
return true;
}

bool ccServer::Delete()
{
static const char *Main = "DELETE FROM servers WHERE lower(Name) = '";

if(!dbConnected)
	{
	return false;
	}

strstream theQuery;
theQuery	<< Main
		<< string_lower(Name)
		<< "'" << ends;

elog	<< "ccontrol::Server::Delete> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

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
