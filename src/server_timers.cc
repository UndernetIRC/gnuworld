/**
 * server_timers.cc
 * This is the implementation file for the xServer class.
 * This class is the entity which is the GNUWorld server
 * proper.  It manages network I/O, parsing and distributing
 * incoming messages, notifying attached clients of
 * system events, on, and on, and on.
 *
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
 * $Id: server_timers.cc,v 1.1 2005/01/07 00:10:46 dan_karrels Exp $
 */

#include	<stack>
#include	<map>
#include	<iostream>

#include	"server.h"
#include	"ELog.h"
#include	"ServerTimerHandlers.h"

RCSTAG( "$Id: server_timers.cc,v 1.1 2005/01/07 00:10:46 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::pair ;
using std::endl ;
using std::stack ;

void xServer::registerServerTimers()
{
RegisterTimer( ::time( 0 ) + glineUpdateInterval,
	new GlineUpdateTimer( this, glineUpdateInterval ),
	static_cast< void* >( this ) ) ;
RegisterTimer( ::time( 0 ) + pingUpdateInterval,
	new PINGTimer( this, pingUpdateInterval ),
	static_cast< void* >( this ) ) ;
}

xServer::timerID xServer::RegisterTimer( const time_t& absTime,
	TimerHandler* theHandler,
	void* data )
{
assert( theHandler != 0 ) ;

// Allow registration of timers which are requesting to be executed
// at times which are <= now.
// This will just allow the timer to run on the next iteration

// Retrieve a unique timerID
timerID ID = getUniqueTimerID() ;

// Allocate a timerInfo structure to represent this timer
timerInfo* ti =
	new (std::nothrow) timerInfo( ID, absTime, theHandler, data ) ;
assert( ti != 0 ) ;

// Add this timerInfo structure to the timerQueue
timerQueue.push( timerQueueType::value_type( absTime, ti ) ) ;

// Add the unique timerID to the timerID map
uniqueTimerMap.insert( uniqueTimerMapType::value_type( ID, true ) ) ;

// Return the valid timerID of this timer
return ID ;
}

bool xServer::UnRegisterTimer( const xServer::timerID& ID,
	void* data )
{
// Make sure there are timers in the queue
if( timerQueue.empty() )
	{
	// The timerQueue is empty -- the timerID specified
	// is invalid at best.
	return true ;
	}

// Create a local stack to store elements popped from the timerQueue
// in searching for the timer in question (could also have used
// another priority queue)
stack< timerQueueType::value_type > localStack ;

// Continue while the timerQueue is not empty, and we have not
// found the timerID in question.
while( !timerQueue.empty() && (timerQueue.top().second->ID != ID) )
	{
	// Add this timer to the local stack
	localStack.push( timerQueue.top() ) ;

	// Remove this timer from the timerQueue
	timerQueue.pop() ;
	}

// This variable will represent the case that we found
// the timer in question
bool foundTimer = false ;

// timerQueue is now empty, or its top element has the timer
// we are interested in.
if( !timerQueue.empty() )
	{
	// Find was successful
	foundTimer = true ;

	// Assign the timer argument back to "data"
	if( data != 0 )
		{
		data = timerQueue.top().second->data ;
		}

	// Remove this timerID from the uniqueTimerMap
	uniqueTimerMap.erase( timerQueue.top().second->ID ) ;

	// Deallocate the timerInfo structure for this timer
	delete timerQueue.top().second ;

	// Remove the timerInfo from the timerQueue
	timerQueue.pop() ;
	}

// Put the rest of the timers back onto the timerQueue.
while( !localStack.empty() )
	{
	timerQueue.push( localStack.top() ) ;
	localStack.pop() ;
	}

return foundTimer ;
}

unsigned int xServer::CheckTimers()
{
// Make sure the timerQueue is not empty
if( timerQueue.empty() )
	{
	return 0 ;
	}

// Create a variable to count the number of timers executed
unsigned int numTimers = 0 ;

// What time is this method being invoked?
time_t now = ::time( 0 ) ;

while( !timerQueue.empty() && (timerQueue.top().second->absTime <= now) )
	{
	// Grab a timerInfo structure
	timerInfo* info = timerQueue.top().second ;

	// Remove the structure from the timerQueue
	timerQueue.pop() ;

	// Call the timer handler method for the client
	info->theHandler->OnTimer( info->ID, info->data ) ;

	// Remove the timerID from the uniqueTimerMap
	uniqueTimerMap.erase( info->ID ) ;

	// Deallocate the timerInfo structure for this timer
	delete info ;

	// Increment the counter for number of timers executed
	++numTimers ;
	}

// Return the number of timers executed
return numTimers ;
}

xServer::timerID xServer::getUniqueTimerID()
{
timerID retMe = lastTimerID++ ;
while( uniqueTimerMap.find( retMe ) != uniqueTimerMap.end() )
	{
	retMe = lastTimerID++ ;
	}
return retMe ;
}

void xServer::removeAllTimers( TimerHandler* theHandler )
{
// Stack all timerInfo structures that do not correspond
// to the given TimerHandler.
std::stack< pair< time_t, timerInfo* > > theStack ;

while( !timerQueue.empty() )
	{
	pair< time_t, timerInfo* > thePair = timerQueue.top() ;
	timerQueue.pop() ;

	if( thePair.second->theHandler == theHandler )
		{
		// This timerInfo belongs to the TimerHandler in
		// question.
//		elog	<< "xServer::removeAllTimers> Found "
//			<< "a timer that was not unregistered"
//			<< endl ;

		// Since the TimerHandler has little or no access
		// to the timer system internals here, it is safe
		// to simply call its OnTimerDestroy() method, even
		// though that method may call other xServer methods.
		theHandler->OnTimerDestroy( thePair.first, thePair.second ) ;

		delete thePair.second ;
		}
	else
		{
		// Add it to the stack
		theStack.push( thePair ) ;
		}
	 } // while( !empty )

// All done, now put the timers back into the timerQueue
while( !theStack.empty() )
	{
	timerQueue.push( theStack.top() ) ;
	theStack.pop() ;
	}
}

} // namespace gnuworld
