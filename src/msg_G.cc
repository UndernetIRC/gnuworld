/**
 * msg_G.cc
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
 * $Id: msg_G.cc,v 1.3 2002/05/27 17:18:13 dan_karrels Exp $
 */

#include	<string>

#include	"server.h"
#include	"xparameters.h"
#include	"ELog.h"

const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char msg_G_cc_rcsId[] = "$Id:" ;

using std::string ;
using std::endl ;

namespace gnuworld
{

// Q G :ripper.ufl.edu
// Q: Remote server numeric
// G: PING
// :ripper.ufl.edu: Ping argument
// Reply with:
// <Our Numeric> Z <Their Numeric> :<arguments>
// Strings will be passed to this method in format:
// Q ripper.ufl.edu
int xServer::MSG_G( xParameters& params )
{
if( params.size() < 2 )
	{
	elog	<< "xServer::MSG_G> Invalid number of parameters"
		<< endl ;
	return -1 ;
	}

string s( charYY ) ;
s += " Z " ;
s += params[ 0 ] ;

if( params.size() >= static_cast< xParameters::size_type >( 1 ) )
	{
	s += " :" ;
	s += params[ 1 ] ;
	}

return Write( s ) ;
}

} // namespace gnuworld
