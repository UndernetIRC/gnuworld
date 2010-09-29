/**
 * ccGline.cc
 * Gline class
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
 * $Id: ccGline.cc,v 1.22 2007/08/28 16:10:06 dan_karrels Exp $
 */
 
#include	<sstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"dbHandle.h"
#include	"ELog.h"
#include	"misc.h"
#include	"ccGline.h" 
#include	"ccontrol.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: ccGline.cc,v 1.22 2007/08/28 16:10:06 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::stringstream ;
using std::ends ;

namespace uworld
{

unsigned int ccGline::numAllocated = 0;

ccGline::ccGline(dbHandle* _SQLDb)
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
		<< ccontrol::removeSqlChars(string_lower(Host)) << "'"
		<< ends;


if( !SQLDb->Exec( delQuery ) )
//if( PGRES_COMMAND_OK != status ) 
	{
	elog	<< "ccGline::DeleteOnInsert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}
//Now insert the new one
static const char *Main = "INSERT INTO Glines (Host,AddedBy,AddedOn,ExpiresAt,LastUpdated,Reason) VALUES ('";

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(Host) << "','"
		<< ccontrol::removeSqlChars(AddedBy) << "',"
		<< AddedOn << ","
		<< Expires << ","
		<< LastUpdated << ",'"
		<< ccontrol::removeSqlChars(Reason) << "')"
		<< ends;

elog	<< "Gline::Insert::sqlQuery> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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
if(atoi(Id.c_str()) == -1) //saveGlines was false when this gline was added
	{
	return true;
	} 
static const char *Main = "UPDATE Glines SET Id = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< Id
		<< "', Host = '"
		<< ccontrol::removeSqlChars(Host)
		<< "', AddedBy = '"
		<< ccontrol::removeSqlChars(AddedBy)
		<< "', AddedOn = "
		<< AddedOn
		<< ",ExpiresAt = "
		<< Expires
		<< ",LastUpdated = "
		<< LastUpdated
		<<  ",Reason = '"
		<< ccontrol::removeSqlChars(Reason) << "'"
		<< " WHERE Id = " << Id
		<<  ends;

elog	<< "ccontrol::Gline::Update> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
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
AddedOn = static_cast< time_t >( unsigned(
	atoi( SQLDb->GetValue(0,3).c_str() ) )) ;
Expires = static_cast< time_t >( unsigned(
	atoi( SQLDb->GetValue(0,4).c_str() ) )) ;
LastUpdated = static_cast< time_t >( unsigned(
	atoi( SQLDb->GetValue(0,5).c_str() ) )) ;
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
		<< ccontrol::removeSqlChars(HostName.c_str())
		<< "'" << ends;

elog	<< "ccontrol::loadData> "
	<< theQuery.str().c_str()
	<< endl; 

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
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
AddedOn = static_cast< time_t >( atoi( SQLDb->GetValue(0,3).c_str() ) ) ;
Expires = static_cast< time_t >( atoi( SQLDb->GetValue(0,4).c_str() ) ) ;
LastUpdated = static_cast< time_t >( atoi( SQLDb->GetValue(0,5).c_str() ) ) ;
Reason = SQLDb->GetValue(0,6);

return true;
}

bool ccGline::Delete()
{
if(atoi(Id.c_str()) == -1) //saveGlines was false when this gline was added
	{
	return true;
	} 

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

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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


}
} //Namespace Gnuworld
