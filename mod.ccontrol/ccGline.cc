/* 
 * ccGline.cc
 * 
 * Gline class
 * 
 * $Id: ccGline.cc,v 1.14 2002/12/28 22:44:55 mrbean_ Exp $
 */
 
#include	<sstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"libpq++.h"
#include	"ELog.h"
#include	"misc.h"
#include	"ccGline.h" 
#include	"ccontrol.h"

const char ccGline_h_rcsId[] = __CCGLINE_H ;
const char ccGline_cc_rcsId[] = "$Id: ccGline.cc,v 1.14 2002/12/28 22:44:55 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::stringstream ;
using std::ends ;

namespace uworld
{

unsigned int ccGline::numAllocated = 0;

ccGline::ccGline(PgDatabase* _SQLDb)
 : Id(),
   AddedBy(),
   AddedOn( 0 ),
   Expires( 0 ),
   LastUpdated( 0 ),
   Reason(),
   SQLDb( _SQLDb )
{
++numAllocated;
}

ccGline::~ccGline()
{
clearBurst(); //Clear the bursting list
--numAllocated;
}

bool ccGline::Insert()
{
//First we gotta make sure, there is no old gline in the database
static const char *Del = "DELETE FROM glines WHERE lower(host) = '";

if(!dbConnected)
	{
	return false;
	}
stringstream delQuery;
delQuery	<< Del
		<< string_lower(Host) << "'"
		<< ends;


ExecStatusType status = SQLDb->Exec( delQuery.str().c_str() ) ;

if( PGRES_COMMAND_OK != status ) 
	{
	elog	<< "ccGline::DeleteOnInsert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}
//Now insert the new one
static const char *Main = "INSERT into Glines (Host,AddedBy,AddedOn,ExpiresAt,LastUpdated,Reason) VALUES ('";

stringstream theQuery;
theQuery	<< Main
		<< Host << "','"
		<< AddedBy << "',"
		<< AddedOn << ","
		<< Expires << ","
		<< LastUpdated << ",'"
		<< Reason << "')"
		<< ends;

elog	<< "Gline::Insert::sqlQuery> "
	<< theQuery.str().c_str()
	<< endl; 

status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccontrol::Gline::Insert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}

}

bool ccGline::Update()
{
static const char *Main = "UPDATE Glines SET Id = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< Id
		<< "', Host = '"
		<< Host
		<< "', AddedBy = '"
		<< AddedBy
		<< "', AddedOn = "
		<< AddedOn
		<< ",ExpiresAt = "
		<< Expires
		<< ",LastUpdated = "
		<< LastUpdated
		<<  ",Reason = '"
		<< Reason << "'"
		<< " WHERE Id = " << Id
		<<  ends;

elog	<< "ccontrol::Gline::Update> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccontrol::Gline::Update> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
}

bool ccGline::loadData(int GlineId)
{
static const char *Main = "SELECT Id,Host,AddedBy,AddedOn,ExpiresAt,LastUpdated,Reason FROM glines WHERE Id = ";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< GlineId
		<< ends;

elog	<< "ccontrol::glineload> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccGline::load> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}

if(SQLDb->Tuples() < 6)
    return false;
Id = SQLDb->GetValue(0,0);
Host = SQLDb->GetValue(0,1);
AddedBy = SQLDb->GetValue(0,2) ;
AddedOn = static_cast< time_t >( atoi( SQLDb->GetValue(0,3) ) ) ;
Expires = static_cast< time_t >( atoi( SQLDb->GetValue(0,4) ) ) ;
LastUpdated = static_cast< time_t >( atoi( SQLDb->GetValue(0,5) ) ) ;
Reason = SQLDb->GetValue(0,6);

return true;
}

bool ccGline::loadData( const string & HostName)
{
static const char *Main = "SELECT Id,Host,AddedBy,AddedOn,ExpiresAt,LastUpdated,Reason FROM glines WHERE Host = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< HostName.c_str()
		<< "'" << ends;

elog	<< "ccontrol::loadData> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccGline::loadData> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}

if(SQLDb->Tuples() == 0) //If no gline was found
	return false;
Id = SQLDb->GetValue(0,0);
Host = SQLDb->GetValue(0,1);
AddedBy = SQLDb->GetValue(0,2) ;
AddedOn = static_cast< time_t >( atoi( SQLDb->GetValue(0,3) ) ) ;
Expires = static_cast< time_t >( atoi( SQLDb->GetValue(0,4) ) ) ;
LastUpdated = static_cast< time_t >( atoi( SQLDb->GetValue(0,5) ) ) ;
Reason = SQLDb->GetValue(0,6);

return true;
}

bool ccGline::Delete()
{
static const char *Main = "DELETE FROM glines WHERE Id = ";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< Id
		<< ends;

elog	<< "ccontrol::glineDelete> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccGline::ccDelete> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}
return true;
}

void ccGline::addBurst(string *Server)
{
burstIterator ptr = burstServers.begin();
for(;ptr != burstServers.end();++ptr)
	{
	if(!strcmp((**ptr).c_str(),Server->c_str()))
		break;
	}
if(ptr == burstServers.end())
	{
	burstServers.push_back(Server);
	}
}

void ccGline::clearBurst()
{
burstIterator ptr = burstServers.begin();
for(;ptr != burstServers.end();)
	{
	delete *ptr;
	ptr = burstServers.erase(ptr);
	}
}

}
} //Namespace Gnuworld
