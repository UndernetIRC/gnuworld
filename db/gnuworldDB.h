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
 * $Id: gnuworldDB.h,v 1.3 2002/05/31 15:07:19 dan_karrels Exp $
 */

#ifndef __GNUWORLDDB_H
#define __GNUWORLDDB_H "$Id: gnuworldDB.h,v 1.3 2002/05/31 15:07:19 dan_karrels Exp $"

#include	<string>
#include	<sstream>

#include	<sys/types.h>

namespace gnuworld
{

using std::stringstream ;
using std::string ;

class gnuworldDB
{

protected:
	string			dbHost ;
	unsigned short int	dbPort ;
	string			dbName ;
	string			userName ;
	string			password ;

public:

	gnuworldDB( const string& dbHost,
		const unsigned short int dbPort,
		const string& dbName,
		const string& userName,
		const string& password ) ;
	virtual ~gnuworldDB() ;

	virtual bool		Exec( const stringstream& ) = 0 ;
	virtual bool		Exec( const string& ) = 0 ;
	virtual bool		isConnected() const = 0 ;

	virtual unsigned int	countTuples() const = 0 ;

	virtual const string	ErrorMessage() const = 0 ;
	virtual const string	GetValue( const unsigned int&,
					const unsigned int& ) const = 0 ;

	inline const string&	getDBHost() const
		{ return dbHost ; }
	inline const unsigned short int	getDBPort() const
		{ return dbPort ; }
	inline const string&	getDBName() const
		{ return dbName ; }
	inline const string&	getUserName() const
		{ return userName ; }
	inline const string&	getPassword() const
		{ return password ; }

protected:


} ;

} // namespace gnuworld

#endif // __GNUWORLDDB_H
