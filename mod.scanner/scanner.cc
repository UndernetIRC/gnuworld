/**
 * scanner.cc
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
 * $Id: scanner.cc,v 1.2 2002/08/07 20:55:22 dan_karrels Exp $
 */

#include	"client.h"
#include	"scanner.h"
#include	"server.h"
#include	"EConfig.h"
#include	"ELog.h"
#include	"iClient.h"

namespace gnuworld
{

/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */
extern "C"
{
  xClient* _gnuwinit(const string& args)
  { 
    return new scanner( args );
  }

} 
 
/**
 * This constructor calls the base class constructor.  The xClient
 * constructor will open the configuration file given and retrieve
 * basic client info (nick/user/host/etc).
 * Any additional processing must be done here.
 */
scanner::scanner( const string& configFileName )
 : xClient( configFileName )
{}

scanner::~scanner()
{
/* No heap space allocated */
}

void scanner::ImplementServer( xServer* theServer )
{
xClient::ImplementServer( theServer ) ;

theServer->RegisterEvent( EVT_NICK, this ) ;
}

int scanner::OnPrivateMessage( iClient* theClient,
	const string& message, bool )
{
Notice( theClient, "Howdy :)" ) ;
return 0 ;
}

// Burst any channels.
int scanner::BurstChannels()
{
Join( "#some_oper_channel" ) ;
return 0 ;
}

int scanner::OnEvent( const eventType& whichEvent,
	void* arg1,
	void* arg2,
	void* arg3,
	void* arg4 )
{
switch( whichEvent )
	{
	case EVT_NICK:
		handleNewClient( static_cast< iClient* >( arg1 ) ) ;
		break ;
	default:
		elog	<< "scanner::OnEvent> Received unknown event: "
			<< whichEvent
			<< endl ;
		break ;
	}
return xClient::OnEvent( whichEvent, arg1, arg2, arg3, arg4 ) ;
}

void scanner::handleNewClient( iClient* newClient )
{


}

} // namespace gnuworld
