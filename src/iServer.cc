/**
 * iServer.cc
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
 * $Id: iServer.cc,v 1.7 2003/06/17 15:13:54 dan_karrels Exp $
 */

#include	<string>
#include	<ctime>

#include	"iServer.h"
#include	"Numeric.h"
#include	"config.h"

RCSTAG( "$Id: iServer.cc,v 1.7 2003/06/17 15:13:54 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

iServer::iServer( const unsigned int& _uplink,
	const string& _yxx,
	const string& _name,
	const time_t& _connectTime )
: uplinkIntYY( _uplink ),
	name( _name ),
	connectTime( _connectTime ),
	bursting( false )
{
// yyxxx, n2k
intYY = base64toint( _yxx.c_str(), 2 ) ;
inttobase64( charYY, intYY, 2 ) ;

intXXX = base64toint( _yxx.c_str() + 2, 3 ) ;
}

iServer::~iServer()
{}

} // namespace gnuworld
