/**
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
 * "$Id: msg_AC.cc,v 1.3 2002/07/05 01:10:05 dan_karrels Exp $"
 */

#include	"ServerCommandHandler.h"
#include	"server.h"
#include	"xparameters.h"
#include	"Channel.h"
#include	"Network.h"
#include	"iClient.h"

const char ServerCommandHandler_cc_rcsId[] = __SERVERCOMMANDHANDLER_H ;
const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char msg_AC_cc_rcsId[] = "$Id: msg_AC.cc,v 1.3 2002/07/05 01:10:05 dan_karrels Exp $" ;

namespace gnuworld
{

CREATE_HANDLER(msg_AC)

/**
 * ACCOUNT message handler.
 * SOURCE AC TARGET ACCOUNT
 * Eg:
 * AXAAA AC BQrTd Gte
 */
bool msg_AC::Execute( const xParameters& Param )
{
// Find the target user
iClient* theClient = Network->findClient(Param[1]);
if( !theClient )
	{
	return false;
	}

// Update user information
theClient->setAccount( Param[ 2 ] ) ;

// Post event to listening clients
theServer->PostEvent( EVT_ACCOUNT, static_cast< void* >( theClient ) ) ;

// Return success
return true;
}

} // namespace gnuworld
