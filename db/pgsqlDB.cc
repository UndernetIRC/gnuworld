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
 * $Id: pgsqlDB.cc,v 1.2 2002/05/31 15:07:19 dan_karrels Exp $
 */

#include	<sys/types.h>

#include	<new>
#include	<exception>
#include	<sstream>
#include	<string>

#include	"libpq++.h"
#include	"gnuworldDB.h"
#include	"pgsqlDB.h"

const char pgsqlDB_cc_rcsId[] = "$Id: pgsqlDB.cc,v 1.2 2002/05/31 15:07:19 dan_karrels Exp $" ;
const char pgsqlDB_h_rcsId[] = __PGSQLDB_H ;

namespace gnuworld
{

using std::ends ;
using std::string ;
using std::stringstream ;

pgsqlDB::pgsqlDB( const string& dbHost,
	const unsigned short int dbPort,
	const string& dbName,
	const string& userName,
	const string& password ) throw( std::exception )
: gnuworldDB( dbHost, dbPort, dbName, userName, password )
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
theDB = new PgDatabase( s.str().c_str() ) ;

if( theDB->ConnectionBad() )
	{
	throw std::exception() ;
	}
}

pgsqlDB::~pgsqlDB()
{
delete theDB ; theDB = 0 ;
}

bool pgsqlDB::Exec( const string& theQuery )
{
return (PGRES_COMMAND_OK == theDB->Exec( theQuery.c_str() )) ;
}

bool pgsqlDB::Exec( const stringstream& theQuery )
{
return (PGRES_COMMAND_OK == theDB->Exec( theQuery.str().c_str() )) ;
}

unsigned int pgsqlDB::countTuples() const
{
return theDB->Tuples() ;
}

const string pgsqlDB::ErrorMessage() const
{
return theDB->ErrorMessage() ;
}

const string pgsqlDB::GetValue( const unsigned int& tupleNumber,
	const unsigned int& fieldNumber ) const
{
return theDB->GetValue( tupleNumber, fieldNumber ) ;
}

} // namespace gnuworld
