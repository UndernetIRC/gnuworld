/**
 * msg_RO.cc
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
 * $Id: msg_RO.cc,v 1.1 2009/07/25 18:12:34 hidden1 Exp $
 */

#include	<iostream>
#include	<string>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"

RCSTAG( "$Id: msg_RO.cc,v 1.1 2009/07/25 18:12:34 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::endl ;
using std::string ;

CREATE_HANDLER(msg_RO)

bool msg_RO::Execute( const xParameters& Param )
{
if( Param.empty() )
	{
	elog	<< "msg_RO> Invalid number of "
		<< "arguments"
		<< endl ;
	return false ;
	}

xClient* theClient = Network->findLocalClient( Param[ 2 ] ) ;
if( NULL == theClient )
	{
	elog	<< "msg_RO> Unable to find nick: "
		<< Param[ 1 ]
		<< endl ;
	return false ;
	}

iServer* tmpServer = Network->findServer(Param[0]);
if( NULL == tmpServer )
	{
	elog	<< "msg_RO> Unable to find server: "
		<< Param[ 0 ]
		<< endl ;
	return false ;
	}

string tMessage = string( tmpServer->getCharYY() ) + " RO " + theServer->getCharYY() + " ";
for( xParameters::size_type i = 2 ; i < Param.size() ; ++i )
	{
	tMessage += Param[ i ] ;
	if( (i + 1) < Param.size() )
		{
		tMessage += " " ;
		}
	}

theClient->OnServerMessage( tmpServer, tMessage );

return true ;
}

} // namespace gnuworld
