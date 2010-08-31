/**
 * msg_XQ.cc
 * Copyright (C) 2010 Jochen Meesters <ekips@pandora.be>
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
 * $Id: msg_XQ.cc,v 1.1 2010/08/31 21:16:46 denspike Exp $
 */

#include	<string>
#include	<iostream>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"Network.h"
#include	"iClient.h"
#include	"client.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"
#include	"StringTokenizer.h"

namespace gnuworld
{

using std::string ;
using std::endl ;

CREATE_HANDLER(msg_XQ)

/**
 * A server or oper sends an XQUERY
 * <prefix> XQ <target> <routing> :<query>
 *
 * <prefix>: Originating server numeric, can be an oper for debugging purposes
 * <target>: Our servernumeric
 * <routing>: Token to supply in XR
 * <query>: Message, can be spaced
 * 
 * [IN ]: ABAAA XQ Az tokengoeshere :message goes here :)
 */
bool msg_XQ::Execute( const xParameters& Param )
{
if( Param.size() < 4 )
	{
	elog	<< "msg_XQ> Invalid number of arguments"
		<< endl ;
	return false ;
	}

iServer* serverSource = 0 ;
iClient* clientSource = 0 ;

if( strlen( Param[ 0 ] ) >= 3 )
        {
        clientSource = Network->findClient( Param[ 0 ] ) ;
	//This is an oper debugging, treat messages as originated from his server
	serverSource = Network->findServer(clientSource->getIntYY());
        }
else
        {
        serverSource = Network->findServer( Param[ 0 ] ) ;
        }

if( (NULL == clientSource) && (NULL == serverSource) )
        {
        elog    << "msg_XQ> Unable to find source: "
                << Param[ 0 ]
                << endl ;
        return false ;
        }

if (Network->findServer(Param[ 1 ]) != theServer->getMe())
	{
	//Should we do something here?
        elog    << "msg_XQ> Received XQ not meant for us but for: "
                << Param[ 1 ]
                << endl ;
        return false ;
        }

elog 	<< "msg_XQ> Received, from: "
	<< Param[ 0 ] << " To: "
	<< Param[ 1 ] << " Token: "
	<< Param[ 2 ] << " Message: "
	<< Param[ 3 ]
	<< endl;

string Routing( Param[2] );
string Message( Param[3] );

theServer->OnXQuery(serverSource, Routing, Message);
//string Routing = Param[2];
//string Message = Param[3];
//elog << "STRINGS:: " << Routing << " " << Message << endl;

//theServer->PostEvent( EVT_XQUERY,
//	static_cast< void* >( serverSource ),
//	reinterpret_cast< void* > ( &Routing ), reinterpret_cast< void* >( &Message ));

return true;
} // msg_XQ

} // namespace gnuworld
