/**
 * Signal.cc
 * Author: Daniel Karrels dan@karrels.com
 * Copyright (C) 2003 Daniel Karrels <dan@karrels.com>
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
 * $Id: Signal.cc,v 1.4 2003/06/17 15:13:53 dan_karrels Exp $
 */

#include	<pthread.h>
#include	<signal.h>
#include	<errno.h>

#include	<queue>
#include	<iostream>

#include	<cerrno>

#include	"Signal.h"

const char rcsId[] = "$Id: Signal.cc,v 1.4 2003/06/17 15:13:53 dan_karrels Exp $" ;

namespace gnuworld
{

using std::queue ;
using std::cout ;
using std::endl ;

queue< int >	Signal::signals1 ;
queue< int >	Signal::signals2 ;
pthread_mutex_t	Signal::signals1Mutex = PTHREAD_MUTEX_INITIALIZER ;

// This will need to be changed to work properly in some code.
//Signal sig ;

Signal::Signal()
{
::pthread_mutex_init( &signals1Mutex, 0 ) ;

// Catch some signals
#ifdef SIGINT
  ::signal( SIGINT, AddSignal ) ;
#endif

#ifdef SIGHUP
  ::signal( SIGHUP, AddSignal ) ;
#endif

#ifdef SIGPIPE
  ::signal( SIGPIPE, AddSignal ) ;
#endif

#ifdef SIGTERM
  ::signal( SIGTERM, AddSignal ) ;
#endif

#ifdef SIGPOLL
  ::signal( SIGPOLL, AddSignal ) ;
#endif

#ifdef SIGUSR1
  ::signal( SIGUSR1, AddSignal ) ;
#endif

#ifdef SIGUSR2
  ::signal( SIGUSR2, AddSignal ) ;
#endif

}

Signal::~Signal()
{
::pthread_mutex_destroy( &signals1Mutex ) ;
}

void Signal::AddSignal( int whichSig )
{
if( EBUSY == ::pthread_mutex_trylock( &signals1Mutex ) )
	{
	// Another thread is using the first queue
	// Add this signal to the second
	signals2.push( whichSig ) ;

	cout	<< "AddSignal> Added signal: "
		<< whichSig
		<< " to signals2"
		<< endl ;
	}
else
	{
	// We have the lock on the signalsMutex
	signals1.push( whichSig ) ;
	::pthread_mutex_unlock( &signals1Mutex ) ;

	cout	<< "AddSignal> Added signal: "
		<< whichSig
		<< " to signals1"
		<< endl ;
	}
}

int Signal::getSignal()
{
int retMe = -1 ;

// Make sure to lock the mutex for signals1
// If a signal handler encounters a locked mutex, it
// will continue on to modify signals2.
::pthread_mutex_lock( &signals1Mutex ) ;

if( !signals1.empty() )
	{
	retMe = signals1.front() ;
	signals1.pop() ;
	::pthread_mutex_unlock( &signals1Mutex ) ;

	return retMe ;
	}
::pthread_mutex_unlock( &signals1Mutex ) ;

// Nothing found in the signals1
// Now that this thread does not have the lock on the mutex,
// any signal handlers will lock it and modify signals1.
// We are safe to modify signals2
if( !signals2.empty() )
	{
	retMe = signals2.front() ;
	signals2.pop() ;
	}
return retMe ;
}

} // namespace gnuworld
