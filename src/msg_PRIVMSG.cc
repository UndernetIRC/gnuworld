/**
 * msg_PRIVMSG.cc
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
 * $Id: msg_PRIVMSG.cc,v 1.4 2002/05/27 17:18:13 dan_karrels Exp $
 */

#include	"server.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_PRIVMSG_cc_rcsId[] = "$Id: msg_PRIVMSG.cc,v 1.4 2002/05/27 17:18:13 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::endl ;

// This is a blatant hack until ircu gets its protocol straight
int xServer::MSG_PRIVMSG( xParameters& Param )
{
if( Param.empty() )
	{
	elog	<< "xServer::MSG_PRIVMSG> Invalid number of "
		<< "arguments"
		<< endl ;
	return -1 ;
	}

// Dont try this at home kids
char numeric[ 6 ] = { 0 } ;

iClient* theClient = Network->findNick( Param[ 0 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_PRIVMSG> Unable to find nick: "
		<< Param[ 0 ]
		<< endl ;
	return -1 ;
	}

strcpy( numeric, theClient->getCharYYXXX().c_str() ) ;
Param.setValue( 0, numeric ) ;

return MSG_P( Param ) ;
}

} // namespace gnuworld
