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
 * $Id: iServer.cc,v 1.11 2006/12/22 06:41:45 kewlio Exp $
 */

#include	<string>
#include	<ctime>

#include	<cstring>

#include	"iServer.h"
#include	"Numeric.h"
#include	"gnuworld_config.h"
#include	"NetworkTarget.h"

RCSTAG( "$Id: iServer.cc,v 1.11 2006/12/22 06:41:45 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

const iServer::flagType iServer::FLAG_JUPE	= 0x01 ;
const iServer::flagType iServer::FLAG_HUB	= 0x02 ;
const iServer::flagType iServer::FLAG_SERVICE	= 0x04 ;
const iServer::flagType iServer::FLAG_IPV6	= 0x08 ;

iServer::iServer( const unsigned int& _uplink,
	const string& _yyxxx,
	const string& _name,
	const time_t& _connectTime,
	const string& _description )
: NetworkTarget( _yyxxx ),
	uplinkIntYY( _uplink ),
	name( _name ),
	connectTime( _connectTime ),
	startTime( _connectTime ),
	description( _description ),
	bursting( false ),
	flags( 0 )
{}

iServer::~iServer()
{}

/**
 * Interpret a server's flags.
 *
 * @Param[in] newFlags String listing server's P10 flags.
 */
void iServer::setFlags( const string& newFlags )
{
	for( string::size_type i = 0 ; i < newFlags.size() ; i++ )
	{
		switch( newFlags[ i ] )
		{
			case 'h':
					setHub() ;
					break ;
			case 's':
					setService() ;
					break ;
			case '6':
					setIPv6() ;
					break ;
			case '+':	break ;
			default:
					// Unknown flag
					elog	<< "iServer> Unknown server flag: "
						<< newFlags[ i ]
						<< ", in flags string: "
						<< newFlags
						<< std::endl ;
					break ;
		}
	}
}

} // namespace gnuworld
