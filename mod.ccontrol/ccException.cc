/**
 * ccException.cc
 * Exception class
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
 * $Id: ccException.cc,v 1.15 2007/08/28 16:10:06 dan_karrels Exp $
 */
 
#include	<sstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"ELog.h"
#include	"misc.h"
#include	"match.h"
#include	"ccException.h" 
#include	"ccontrol.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: ccException.cc,v 1.15 2007/08/28 16:10:06 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::stringstream ;
using std::ends ;

namespace uworld
{

//extern unsigned int dbConnected;
unsigned int ccException::numAllocated = 0;

ccException::ccException(dbHandle* _SQLDb)
 : Host(string()),
   Connections(0),
   AddedBy(string()),
   AddedOn(0),
   Reason(string()),
   SQLDb(_SQLDb)
{
++numAllocated;
}

ccException::~ccException()
{
--numAllocated;
}

int ccException::loadData(const string& HostName)
{

static const char Main[] = "SELECT * FROM Exceptions WHERE lower(Host) = '";

if((!dbConnected) || !(SQLDb))
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(string_lower(HostName))
		<< "'"
		<< ends;

elog	<< "ccException::loadData> "
	<< theQuery.str().c_str()
	<< endl; 

// TODO: Isn't this impossible?
if( !SQLDb->Exec( theQuery, true ) && (SQLDb->Tuples() > 0) )
//if( (PGRES_TUPLES_OK != status) && (SQLDb->Tuples() > 0) )
	{
	return false;
	}

Host = SQLDb->GetValue(0,0);
Connections = atoi(SQLDb->GetValue(0,1).c_str());
AddedBy = SQLDb->GetValue(0,2);
AddedOn = atoi(SQLDb->GetValue(0,3).c_str());
Reason = SQLDb->GetValue(0,4);
return true;

}

int ccException::updateData()
{
static const char *Main = "UPDATE Exceptions SET AddedBy = '";

if(!dbConnected)	
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(AddedBy)
		<< "', Connections = "
		<< Connections
		<< ", AddedOn = "
		<< AddedOn
		<< ", Reason = '"
		<< ccontrol::removeSqlChars(Reason)
		<< "' WHERE lower(Host) = '" 
		<< ccontrol::removeSqlChars(string_lower(Host)) << "'"
		<<  ends;

elog	<< "ccException::Update> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccException::Update> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}

}

bool ccException::Insert()
{
static const char *quer = "INSERT INTO exceptions(host,connections,addedby,addedon,reason) VALUES ('";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(Host) << "',"
		<< Connections
		<< ",'" << ccontrol::removeSqlChars(AddedBy)
		<< "'," << AddedOn
		<< ",' " << ccontrol::removeSqlChars(Reason)
		<< "')" << ends;

elog	<< "ccException::Insert> "
	<< query.str().c_str()
	<< endl; 

if( !SQLDb->Exec( query ) )
//if(PGRES_COMMAND_OK != status)
	{
	elog	<< "ccException::Insert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}
return true ;
//return (PGRES_COMMAND_OK == status) ;
}

bool ccException::Delete()
{
static const char *quer = "DELETE FROM exceptions WHERE host = '";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(Host) << "'"
		<< ends;

elog 		<< "ccException::delException> "
		<< query.str().c_str()
		<< endl ;

if( !SQLDb->Exec( query ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccException::findException> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;		    
	}
return true;
}

}
}
