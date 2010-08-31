/**
 * msg_XR.cc
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
 * $Id: msg_XR.cc,v 1.1 2010/08/31 21:16:46 denspike Exp $
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

CREATE_HANDLER(msg_XR)

/**
 * A server replies an XREPLY
 * <prefix> XR <target> <routing> :<query>
 *
 * <prefix>: Originating server numeric, can be an oper for debugging purposes
 * <target>: Our servernumeric
 * <routing>: Token to supply in XR
 * <query>: Message, can be spaced
 * 
 * [IN ]: AB XR Az tokengoeshere :message goes here :)
 */
bool msg_XR::Execute( const xParameters& Param )
{
if( Param.size() < 4 )
	{
	elog	<< "msg_XR> Invalid number of arguments"
		<< endl ;
	return false ;
	}

iServer* serverSource = Network->findServer( Param[ 0 ] ) ; 

if( NULL == serverSource) 
        {
        elog    << "msg_XR> Unable to find source: "
                << Param[ 0 ]
                << endl ;
        return false ;
        }

if (Network->findServer(Param[ 1 ]) != theServer->getMe())
	{
	//Should we do something here?
        elog    << "msg_XR> Received XQ not meant for us but for: "
                << Param[ 1 ]
                << endl ;
        return false ;
        }

elog 	<< "msg_XR> Received, from: "
	<< Param[ 0 ] << " To: "
	<< Param[ 1 ] << " Token: "
	<< Param[ 2 ] << " Message: "
	<< Param[ 3 ]
	<< endl;

string Routing( Param[2] );
string Message( Param[3] );

theServer->OnXReply(serverSource, Routing, Message);
//string Routing = Param[2];
//string Message = Param[3];
//elog << "STRINGS:: " << Routing << " " << Message << endl;

return true;
} // msg_XR

} // namespace gnuworld
