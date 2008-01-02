/**
 * gnuworldDB.cc
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
 * $Id: gnuworldDB.cc,v 1.5 2008/01/02 23:32:17 kewlio Exp $
 */

#include	<string>

#include	"gnuworldDB.h"

const char gnuworldDB_cc_rcsId[] = "$Id: gnuworldDB.cc,v 1.5 2008/01/02 23:32:17 kewlio Exp $" ;
const char gnuworldDB_h_rcsId[] = __GNUWORLDDB_H ;

namespace gnuworld
{
using std::string ;

gnuworldDB::gnuworldDB( )
{
	/* dummy to prevent issues with pedantic compilers */
}

gnuworldDB::gnuworldDB( const string& _dbHost,
	const unsigned short int _dbPort,
	const string& _dbName,
	const string& _userName,
	const string& _password )
 : dbHost( _dbHost ),
	dbPort( _dbPort ),
	dbName( _dbName ),
	userName( _userName ),
	password( _password )
{}

gnuworldDB::gnuworldDB( const string& /* connectInfo */ )
{
// TODO
}

gnuworldDB::~gnuworldDB()
{}

} // namespace gnuworld
