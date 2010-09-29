/**
 * pgsqlDB.cc
 * Author: Daniel Karrels (dan@karrels.com)
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: pgsqlDB.cc,v 1.5 2009/07/25 16:59:48 mrbean_ Exp $
 */

#include	<sys/types.h>

#include	<new>
#include	<iostream>
#include	<exception>
#include	<sstream>
#include	<string>

#include	"libpq-fe.h"
#include	"gnuworldDB.h"
#include	"pgsqlDB.h"

const char pgsqlDB_cc_rcsId[] = "$Id: pgsqlDB.cc,v 1.5 2009/07/25 16:59:48 mrbean_ Exp $" ;
const char pgsqlDB_h_rcsId[] = __PGSQLDB_H ;

namespace gnuworld
{

using std::cout ;
using std::endl ;
using std::ends ;
using std::string ;
using std::stringstream ;

pgsqlDB::pgsqlDB( const string& dbHost,
	const unsigned short int dbPort,
	const string& dbName,
	const string& userName,
	const string& password ) throw( std::exception )
: gnuworldDB( dbHost, dbPort, dbName, userName, password ),
  theDB( 0 ),
  lastResult( 0 )
{
stringstream s ;
s	<< "host=" << dbHost
	<< " dbname=" << dbName
	<< " port=" << dbPort ;

if( !userName.empty() )
	{
	s	<< " user=" << userName ;
	}
if( !password.empty() )
	{
	s	<< " password=" << password ;
	}
s << ends ;

// Allow exception to be thrown
theDB = PQconnectdb( s.str().c_str() ) ;
if( 0 == theDB )
	{
	cout	<< "pgsqlDB> Failed to allocate memory for db handle"
		<< endl ;
	throw std::exception() ;
	}
if( !isConnected() )
	{
	cout	<< "pgsqlDB> Failed to connect to db: "
		<< ErrorMessage()
		<< endl ;
	throw std::exception() ;
	}
}

pgsqlDB::pgsqlDB( const string& connectInfo ) throw( std::exception )
{
// TODO
// Allow exception to be thrown
lastResult = 0;
theDB = PQconnectdb( connectInfo.c_str() ) ;
if( 0 == theDB )
	{
	cout	<< "pgsqlDB> Failed to allocate memory for db handle"
		<< endl ;
	throw std::exception() ;
	}
if( !isConnected() )
	{
	cout	<< "pgsqlDB> Failed to connect to db: "
		<< ErrorMessage()
		<< endl ;
	throw std::exception() ;
	}
}

pgsqlDB::~pgsqlDB()
{
if( theDB != 0 )
	{
	PQfinish( theDB ) ;
	theDB = 0 ;
	}
if( lastResult != 0 )
	{
	PQclear( lastResult ) ;
	lastResult = 0 ;
	}
}

bool pgsqlDB::Exec( const string& theQuery, bool dataRet )
{
// It is necessary to manually deallocate the last result
// to prevent memory leaks.
if( lastResult != 0 )
	{
	PQclear( lastResult ) ;
	lastResult = 0 ;
	}
lastResult = PQexec( theDB, theQuery.c_str() ) ;

ExecStatusType status = PQresultStatus( lastResult ) ;
if( dataRet )
	{
	// User is expecting data back, so return status according
	// to tuples being returned
	if (PGRES_COPY_IN == status) return true;
	if (PGRES_TUPLES_OK == status) return true;
	if (PGRES_COMMAND_OK == status) return true;
	return false;
	}
else
	{
	// Command execution with no data returned.
	if (PGRES_COPY_IN == status) return true;
        if (PGRES_TUPLES_OK == status) return true;
	if (PGRES_COMMAND_OK == status) return true;
        return false;
	}
}

bool pgsqlDB::Exec( const stringstream& theQuery, bool retData )
{
return Exec( theQuery.str(), retData ) ;
}

bool pgsqlDB::StartCopyIn( const string& writeMe )
{
return Exec( writeMe ) ;
}

bool pgsqlDB::StopCopyIn()
{
if( 0 == lastResult )
	{
	return false ;
	}
return (PQputCopyEnd( theDB, 0 ) != -1) ;
}

bool pgsqlDB::PutLine( const string& writeMe )
{
if( 0 == lastResult )
	{
	return false ;
	}
return (PQputline( theDB, writeMe.c_str() ) != -1) ;
}

unsigned int pgsqlDB::countTuples() const
{
if( 0 == lastResult )
	{
	return 0 ;
	}
return PQntuples( lastResult ) ;
}

const string pgsqlDB::ErrorMessage() const
{
return string( PQerrorMessage( theDB ) ) ;
}

const string pgsqlDB::GetValue( unsigned int rowNumber,
	unsigned int columnNumber ) const
{
if( 0 == lastResult )
	{
	return string() ;
	}
return PQgetvalue( lastResult, rowNumber, columnNumber ) ;
}

const string pgsqlDB::GetValue( unsigned int rowNumber,
	const string& columnName ) const
{
if( 0 == lastResult )
	{
	return string( "No result stored" ) ;
	}

// Retrieve the column number for this name.
const int columnNumber = PQfnumber( lastResult, columnName.c_str() ) ;
if( -1 == columnNumber )
	{
	return (string( "No such column: " ) + columnName) ;
	}

return PQgetvalue( lastResult, rowNumber, columnNumber ) ;
}

bool pgsqlDB::isConnected() const
{
return (CONNECTION_OK == PQstatus( theDB )) ;
}

} // namespace gnuworld
