/**
 * ServerTimerHandlers.cc
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
 * $Id: ServerTimerHandlers.cc,v 1.6 2003/05/26 21:44:32 dan_karrels Exp $
 */

#include	"ServerTimerHandlers.h"
#include	"server.h"
#include	"ELog.h"

#include	<ctime>

const char ServerTimerHandlers_h_rcsId[] = __SERVERTIMERHANDLERS_H ;
const char ServerTimerHandlers_cc_rcsId[] = "$Id: ServerTimerHandlers.cc,v 1.6 2003/05/26 21:44:32 dan_karrels Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char ELog_h_rcsId[] = __ELOG_H ;

namespace gnuworld
{

int GlineUpdateTimer::OnTimer( timerID, void* )
{

// Remove any expired glines
theServer->updateGlines() ;

// Re-register this timer
theServer->RegisterTimer( ::time( 0 ) + updateInterval, this, 0 ) ;

return 0 ;
}

int PINGTimer::OnTimer( timerID, void* )
{
string writeMe( theServer->getCharYY() ) ;
writeMe += " G " ;
writeMe += ":I am the King, bow before me!\n" ;

theServer->RegisterTimer( ::time( 0 ) + updateInterval, this, 0 ) ;

// Write to the network, even during bursting
return theServer->WriteDuringBurst( writeMe ) ;
}

} // namespace gnuworld
