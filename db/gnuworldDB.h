/**
 * gnuworldDB.h
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
 * $Id: gnuworldDB.h,v 1.5 2009/07/26 18:30:37 mrbean_ Exp $
 */

#ifndef __GNUWORLDDB_H
#define __GNUWORLDDB_H "$Id: gnuworldDB.h,v 1.5 2009/07/26 18:30:37 mrbean_ Exp $"

#include	<string>
#include	<sstream>

#include	<sys/types.h>

namespace gnuworld
{

class gnuworldDB
{
protected:
	std::string		dbHost ;
	unsigned short int	dbPort ;
	std::string		dbName ;
	std::string		userName ;
	std::string		password ;

public:
	gnuworldDB() ;
	gnuworldDB( const std::string& dbHost,
		const unsigned short int dbPort,
		const std::string& dbName,
		const std::string& userName,
		const std::string& password ) ;
	gnuworldDB( const std::string& connectInfo ) ;
	virtual ~gnuworldDB() ;

	/**
	 * The Exec method will execute an SQL command with the database.
	 * The "returnData" argument is false by default, and indicated
	 * whether or not data is expected back.
	 * If data is expected back, pass true to this method,
	 * and the return value will indicate if data was successfully
	 * returned.
	 * Otherwise, no data is expected back, and the method will
	 * return true if the command was successfully executed
	 * (with no data returned).
	 */
	virtual bool	Exec( const std::stringstream&,
					bool returnData = false ) = 0 ;

	/**
	 * The Exec method will execute an SQL command with the database.
	 * The "returnData" argument is false by default, and indicated
	 * whether or not data is expected back.
	 * If data is expected back, pass true to this method,
	 * and the return value will indicate if data was successfully
	 * returned.
	 * Otherwise, no data is expected back, and the method will
	 * return true if the command was successfully executed
	 * (with no data returned).
	 */
	virtual bool	Exec( const std::string&,
				bool returnData = false ) = 0 ;
	virtual bool	isConnected() const = 0 ;
	virtual bool	ConnectionBad() const
	{ return !isConnected() ; }

	virtual unsigned int	countTuples() const = 0 ;
	virtual unsigned int	Tuples() const
	{ return countTuples() ; }

	virtual const std::string	ErrorMessage() const = 0 ;
	virtual const std::string	GetValue( unsigned int row,
						unsigned int column ) const = 0 ;
	virtual const std::string	GetValue( unsigned int row,
						const std::string& colName )
							const = 0 ;

	virtual bool		PutLine( const std::string& ) = 0 ;
	virtual bool		StartCopyIn( const std::string& ) = 0 ;
	virtual bool		StopCopyIn() = 0 ;

	inline const std::string&	getDBHost() const
		{ return dbHost ; }
	inline const unsigned short int&	getDBPort() const
		{ return dbPort ; }
	inline const std::string&	getDBName() const
		{ return dbName ; }
	inline const std::string&	getUserName() const
		{ return userName ; }
	inline const std::string&	getPassword() const
		{ return password ; }

} ;

} // namespace gnuworld

#endif // __GNUWORLDDB_H
