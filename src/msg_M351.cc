/**
 * msg_M351.cc
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
 * $Id: msg_M351.cc,v 1.5 2002/05/27 17:18:13 dan_karrels Exp $
 */

#include	<iostream>
#include	<string>

#include	"server.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_M351_cc_rcsId[] = "$Id: msg_M351.cc,v 1.5 2002/05/27 17:18:13 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::endl ;
using std::string ;

int xServer::MSG_M351( xParameters& Param )
{
if( Param.empty() )
	{
	elog	<< "xServer::MSG_M351> Invalid number of "
		<< "arguments"
		<< endl ;
	return -1 ;
	}

// Dont try this at home kids
char numeric[ 6 ] = { 0 } ;

xClient* theClient = Network->findLocalNick( Param[ 1 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_M351> Unable to find nick: "
		<< Param[ 1 ]
		<< endl ;
	return -1 ;
	}

strcpy( numeric, theClient->getCharYYXXX().c_str() ) ;
Param.setValue( 1, "351" ) ;
iServer* tmpServer = Network->findServerName(Param[0]);
if( NULL == tmpServer )
	{
	elog	<< "xServer::MSG_M351> Unable to find server: "
		<< Param[ 0 ]
		<< endl ;
	return -1 ;
	}

char numeric2[ 6 ] = { 0 } ;
strcpy( numeric2, tmpServer->getCharYY()) ;
Param.setValue( 0, numeric2 ) ;

string tMessage;
for( xParameters::size_type i = 0 ; i < Param.size() ; ++i )
	{
	tMessage += Param[ i ] ;
	tMessage += " " ;
	}

return theClient->OnServerMessage(tmpServer,tMessage);
}

} // namespace gnuworld
