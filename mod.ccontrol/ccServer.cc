/**
 * ccServer.cc
 * Server class
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
 * $Id: ccServer.cc,v 1.18 2009/08/06 02:59:24 hidden1 Exp $
 */
 
#include	<sstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"dbHandle.h"
#include	"ELog.h"
#include	"misc.h"
#include	"ccServer.h" 
#include	"ccontrol.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: ccServer.cc,v 1.18 2009/08/06 02:59:24 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::stringstream ;
using std::ends ;

namespace uworld
{

unsigned int ccServer::numAllocated = 0;

ccServer::ccServer(dbHandle* _SQLDb)
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
   ReportMissing(true),
   LagTime(0),
   LastLagReport(0),
   LastLagSent(0),
   LastLagRecv(0),
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
static const char *Main = "INSERT INTO servers (Name,LastUplink,LastNumeric,LastConnected,SplitedOn,SplitReason,Version,AddedOn,LastUpdated,ReportMissing) VALUES ('";

if(!dbConnected)
	{
	return false;
	}
stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(Name) <<"','"
		<< ccontrol::removeSqlChars(Uplink) << "','"
		<< ccontrol::removeSqlChars(Numeric) << "',"
		<< LastConnected << ","
		<< LastSplitted 
		<< ",'" << ccontrol::removeSqlChars(SplitReason)
		<< "','" << ccontrol::removeSqlChars(Version)
		<< "'," << AddedOn
		<< "," << LastUpdated
		<< "," 
		<< (ReportMissing ? "'t'" : "'n'")
		<< ")" 
		<< ends;

elog	<< "ccontrol::Server::Insert::sqlQuery> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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
		<< ccontrol::removeSqlChars(Name)
		<< "', LastUplink = '"
		<< ccontrol::removeSqlChars(Uplink)
		<< "', LastNumeric = '"
		<< ccontrol::removeSqlChars(Numeric)
		<< "', LastConnected = "
		<< LastConnected
		<< ",SplitedOn = "
		<< LastSplitted
		<< ", SplitReason = '"
		<< ccontrol::removeSqlChars(SplitReason) 
		<< "', Version = '"
		<< ccontrol::removeSqlChars(Version)
		<< "', AddedOn = " 
		<< AddedOn
		<< ", LastUpdated = " 
		<< LastUpdated
		<< ", ReportMissing = " 
		<< (ReportMissing ? "'t'" : "'n'")
		<< " WHERE lower(Name) = '" << string_lower(Name)
		<<  "'" << ends;

elog	<< "ccontrol::Server::Update> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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
		<< ccontrol::removeSqlChars(string_lower(ServerName))
		<< "'" << ends;

elog	<< "ccontrol::Server::LoadData> "
	<< theQuery.str().c_str()
	<< endl; 

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
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
		<< ccontrol::removeSqlChars(ServNumeric)
		<< "'" << ends;

elog	<< "ccontrol::Server::LoadNumericData> "
	<< theQuery.str().c_str()
	<< endl; 

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
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
LastConnected = static_cast< time_t >( 
	atoi( SQLDb->GetValue(place,2).c_str() ) ) ;
LastSplitted = static_cast< time_t >(
	atoi( SQLDb->GetValue(place,3).c_str() ) ) ;
Numeric = SQLDb->GetValue(place,4);
SplitReason = SQLDb->GetValue(place,5);
Version = SQLDb->GetValue(place,6);
AddedOn = static_cast< time_t >(
	atoi( SQLDb->GetValue(place,7).c_str() ) ) ;
LastUpdated = static_cast< time_t >(
	atoi( SQLDb->GetValue(place,8).c_str() ) ) ;
ReportMissing = (!strcasecmp(SQLDb->GetValue(place,9),"t") ? true : false);
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
		<< ccontrol::removeSqlChars(string_lower(Name))
		<< "'" << ends;

elog	<< "ccontrol::Server::Delete> "
	<< theQuery.str().c_str()
	<< endl; 

if( !SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK != status ) 
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
