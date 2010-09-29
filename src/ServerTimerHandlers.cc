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
 * $Id: ServerTimerHandlers.cc,v 1.11 2005/01/12 03:50:38 dan_karrels Exp $
 */

#include	<string>

#include	<ctime>

#include	"ServerTimerHandlers.h"
#include	"server.h"
#include	"ELog.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: ServerTimerHandlers.cc,v 1.11 2005/01/12 03:50:38 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

void GlineUpdateTimer::OnTimer( const timerID& , void* )
{
// Remove any expired glines
theServer->updateGlines() ;

// Re-register this timer
theServer->RegisterTimer( ::time( 0 ) + updateInterval, this, 0 ) ;
}

void PINGTimer::OnTimer( const timerID& , void* )
{
string writeMe( theServer->getCharYY() ) ;
writeMe += " G :I am the King, bow before me!\n" ;

theServer->RegisterTimer( ::time( 0 ) + updateInterval, this, 0 ) ;

// Write to the network, even during bursting
theServer->WriteDuringBurst( writeMe ) ;
}

} // namespace gnuworld
