/**
 * pgsqlDB.h
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
 * $Id: pgsqlDB.h,v 1.2 2002/05/31 15:07:19 dan_karrels Exp $
 */

#ifndef __PGSQLDB_H
#define __PGSQLDB_H "$Id: pgsqlDB.h,v 1.2 2002/05/31 15:07:19 dan_karrels Exp $"

#include	<sys/types.h>

#include	<string>
#include	<sstream>
#include	<exception>

#include	"libpq++.h"
#include	"gnuworldDB.h"

namespace gnuworld
{

using std::stringstream ;
using std::string ;

class pgsqlDB : public gnuworldDB
{
protected:
	PgDatabase*		theDB ;

public:

	pgsqlDB( const string& dbHost,
		const unsigned short int dbPort,
		const string& dbName,
		const string& userName,
		const string& password ) throw( std::exception ) ;
	virtual ~pgsqlDB() ;

	virtual bool		Exec( const string& ) ;
	virtual bool		Exec( const stringstream& ) ;
	virtual bool		isConnected() const ;

	virtual unsigned int	countTuples() const ;
	virtual const string	ErrorMessage() const ;

	// tuple number, field number
	virtual const string	GetValue( const unsigned int&,
					const unsigned int& ) const ;

protected:


} ;

} // namespace gnuworld

#endif // __PGSQLDB_H
