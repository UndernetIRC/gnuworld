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
 * $Id: msg_G.cc,v 1.4 2002/07/05 01:10:06 dan_karrels Exp $
 */

#include	<string>
#include	<iostream>

#include	"server.h"
#include	"xparameters.h"
#include	"ELog.h"
#include	"ServerCommandHandler.h"

const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char msg_G_cc_rcsId[] = "$Id:" ;

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_G)

// Q G :ripper.ufl.edu
// Q: Remote server numeric
// G: PING
// :ripper.ufl.edu: Ping argument
// Reply with:
// <Our Numeric> Z <Their Numeric> :<arguments>
// Strings will be passed to this method in format:
// Q ripper.ufl.edu
bool msg_G::Execute( const xParameters& params )
{
if( params.size() < 2 )
	{
	elog	<< "msg_G> Invalid number of parameters"
		<< endl ;
	return false ;
	}

string s( theServer->getCharYY() ) ;
s += " Z " ;
s += params[ 0 ] ;

if( params.size() >= static_cast< xParameters::size_type >( 1 ) )
	{
	s += " :" ;
	s += params[ 1 ] ;
	}

return (theServer->Write( s ) > 0) ;
}

} // namespace gnuworld
