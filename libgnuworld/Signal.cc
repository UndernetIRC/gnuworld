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
 * $Id: Signal.cc,v 1.11 2007/09/13 02:00:45 dan_karrels Exp $
 */

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<pthread.h>
#include	<signal.h>
#include	<fcntl.h>
#include	<unistd.h>

#include	<queue>
#include	<iostream>

#include	<cerrno>
#include	<cstdlib>
#include	<cstring>

#include	"Signal.h"
#include	"ELog.h"

const char rcsId[] = "$Id: Signal.cc,v 1.11 2007/09/13 02:00:45 dan_karrels Exp $" ;

namespace gnuworld
{
using std::queue ;
using std::cout ;
using std::endl ;

// This will need to be changed to work properly in some code.
//Signal sig ;

bool Signal::signalError	= false ;
int Signal::readFD		= -1 ;
int Signal::writeFD		= -1 ;

Signal*	Signal::theInstance	= 0 ;

pthread_mutex_t Signal::singletonMutex = PTHREAD_MUTEX_INITIALIZER ;
pthread_mutex_t Signal::pipeMutex = PTHREAD_MUTEX_INITIALIZER ;

Signal::Signal()
{
// No need to initialize signalError here since it is only initialized
// directly above, and because this class models the Singleton
// pattern.
if( !openPipes() )
	{
	elog	<< "Signal> Failed to open FIFO pipes"
		<< endl ;
	// Failure to initialize pipes on startup is a critical
	// failure.
	::exit( 0 ) ;
	}

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
delete theInstance ; theInstance = 0 ;
closePipes() ;
::pthread_mutex_destroy( &singletonMutex ) ;
::pthread_mutex_destroy( &pipeMutex ) ;
}

bool Signal::isError()
{
return ((-1 == readFD) || (-1 == writeFD) || signalError) ;
}

void Signal::closePipes()
{
::pthread_mutex_lock( &pipeMutex ) ;
::close( readFD ) ;
::close( writeFD ) ;
::pthread_mutex_unlock( &pipeMutex ) ;

signalError = true ;
}

bool Signal::openPipes()
{
int rwFD[ 2 ] = { 0, 0 } ;

// Create the pipe
::pthread_mutex_lock( &pipeMutex ) ;
int pipeRet = ::pipe( rwFD ) ;
::pthread_mutex_unlock( &pipeMutex ) ;

if( pipeRet < 0 )
	{
	elog	<< "Signal::openPipes> pipe() failed: "
		<< strerror( errno )
		<< endl ;

	signalError = true ;
	return false ;
	}

// Set the fd's to non-blocking
::pthread_mutex_lock( &pipeMutex ) ;

for( size_t i = 0 ; i < 2 ; ++i )
	{
	// Get current flags
	int flags = ::fcntl( rwFD[ i ], F_GETFL ) ;
	if( flags < 0 )
		{
		::pthread_mutex_unlock( &pipeMutex ) ;
		elog	<< "Signal::openPipes> Failed to get flags "
			<< "for pipe fd: "
			<< strerror( errno )
			<< endl ;

		closePipes() ;
		return false ;
		}

	// Update flags to indicate non-blocking
	flags |= O_NONBLOCK ;

	// Set new flags
	if( ::fcntl( rwFD[ i ], F_SETFL, flags ) < 0 )
		{
		::pthread_mutex_unlock( &pipeMutex ) ;
		elog	<< "Signal::openPipes> Failed to set flags "
			<< "on pipe fd: "
			<< strerror( errno )
			<< endl ;

		closePipes() ;
		return false ;
		}
	} // for()

// All is well
readFD = rwFD[ 0 ] ;
writeFD = rwFD[ 1 ] ;
::pthread_mutex_unlock( &pipeMutex ) ;

signalError = false ;

return true ;
}

/**
 * AddSignal() will not modify the open/close states of the pipes.
 * If any error is detected, then signalError will be set to true
 * and GetSignal() must handle the rest.
 */
void Signal::AddSignal( int whichSig )
{
errno = 0 ;

// Attempt to write this signal to the pipe.
if( ::write( writeFD, &whichSig, sizeof( int ) ) < 0 )
	{
	// Check for non-blocking type errors.
	if( (EINTR != errno) && (EWOULDBLOCK != errno) &&
		(EINTR != EAGAIN) )
		{
		// Critical error occured, readFD no longer valid
		signalError = true ;

		// Do not close the pipes here, the readFD may be
		// in use.  Let getSignal() handle updating the pipes.

		// Do not attempt to create new pipes inside
		// of the signal handler.
		// The getSignal() below will attempt to reopen
		// the pipes if it detects that one is closed.
		return ;
		} // if( errno )
	} // if( ::read() )

// In general, outputting data during a signal handler is a very
// bad idea.
// This is for testing purposes only.
//std::cerr	<< "Signal::AddSignal> Successfully sent signal: "
//	<< whichSig
//	<< endl ;
} // AddSignal()

/**
 * The semantics of the return statement are a little backwards here:
 * - true indicates that the caller should check the value of theSignal
 *   for either a new signal or an error state (-1)
 * - false indicates that no error and no signal are ready
 */
bool Signal::getSignal( int& theSignal )
{
errno = 0 ;

// Attempt to read the next signal from the pipe
::pthread_mutex_lock( &pipeMutex ) ;
int readResult = ::read( readFD, &theSignal, sizeof( int ) ) ;
::pthread_mutex_unlock( &pipeMutex ) ;

if( readResult < 0 )
	{
	// Check for non-blocking type errors.
	if( (EINTR != errno) && (EWOULDBLOCK != errno) &&
		(EINTR != EAGAIN) )
		{
		// Critical error occured, readFD no longer valid

		// closePipes() will set signalError to true.
		closePipes() ;

		// Attempt to re-open the pipes
		openPipes() ;

		theSignal = -1 ;
		return true ;
		}
	else
		{
		// No signal detected
		return false ;
		}
	}

if( readResult != sizeof( int ) )
	{
	elog	<< "Signal::getSignal> Somehow read "
		<< readResult
		<< " bytes, where sizeof(int) is: "
		<< sizeof( int )
		<< endl ;
	}

// Signal detected, read() returned ok
return true ;
}

Signal* Signal::getInstance()
{
::pthread_mutex_lock( &singletonMutex ) ;
if( 0 == theInstance )
	{
	theInstance = new Signal ;
	}
::pthread_mutex_unlock( &singletonMutex ) ;

return theInstance ;
}

} // namespace gnuworld
