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
 * $Id: pgsqlDB.h,v 1.1 2002/05/29 18:46:11 dan_karrels Exp $
 */

#ifndef __PGSQLDB_H
#define __PGSQLDB_H $Id: pgsqlDB.h,v 1.1 2002/05/29 18:46:11 dan_karrels Exp $

#include	<string>

#include	<sys/types.h>

namespace gnuworld
{

using std::string ;

class pgsqlDB
{

public:

	pgsqlDB() ;
	virtual ~pgsqlDB() ;

	virtual bool		Connect( const string& host,
					unsigned short int port,
					const string& dbName,
					const string& pass ) ;

	virtual bool		Exec( const string& ) ;
	virtual bool		isConnected() const ;

	virtual unsigned int	countTuples() const ;
	virtual pid_t		getPID() const ;

	virtual const string&	ErrorMessage() const ;
	virtual const string&	GetValue( const unsigned int&,
					const unsigned int& ) const ;

protected:


} ;

} // namespace gnuworld

#endif // __PGSQLDB_H
