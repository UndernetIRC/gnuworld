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
 * $Id: pgsqlDB.h,v 1.3 2007/08/28 16:09:59 dan_karrels Exp $
 */

#ifndef __PGSQLDB_H
#define __PGSQLDB_H "$Id: pgsqlDB.h,v 1.3 2007/08/28 16:09:59 dan_karrels Exp $"

#include	<sys/types.h>

#include	<string>
#include	<exception>

#include	"libpq-fe.h"
#include	"gnuworldDB.h"

namespace gnuworld
{

class pgsqlDB : public gnuworldDB
{
protected:
	PGconn			*theDB ;
	PGresult		*lastResult ;

public:
	pgsqlDB( const std::string& dbHost,
		const unsigned short int dbPort,
		const std::string& dbName,
		const std::string& userName,
		const std::string& password ) throw( std::exception ) ;
	pgsqlDB( const std::string& connectInfo ) throw( std::exception ) ;
	virtual ~pgsqlDB() ;

	virtual bool		Exec( const std::string&, bool = false ) ;
	virtual bool		Exec( const std::stringstream&, bool = false ) ;
	virtual bool		isConnected() const ;

	virtual bool		PutLine( const std::string& ) ;
	virtual bool		StartCopyIn( const std::string& ) ;
	virtual bool		StopCopyIn() ;

	virtual unsigned int	countTuples() const ;
	virtual const std::string	ErrorMessage() const ;

	// tuple number, field number (row,col)
	virtual const std::string	GetValue( unsigned int,
						unsigned int ) const ;
	virtual const std::string	GetValue( unsigned int row,
						const std::string& colName )
						const ;

} ;

} // namespace gnuworld

#endif // __PGSQLDB_H
