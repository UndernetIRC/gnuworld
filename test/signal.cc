/**
 * signal.cc
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
 * $Id: signal.cc,v 1.5 2004/05/18 16:51:05 dan_karrels Exp $
 */

#include	<sys/types.h>
#include	<signal.h>
#include	<unistd.h>

#include	<iostream>

#include	<cerrno>
#include	<cstring>
#include	<cctype>
#include	<cstdlib>
#include	<ctime>

#include	"Signal.h"
#include	"ELog.h"

using namespace std ;
using namespace gnuworld ;

struct sigInfo
{
int	signalID ;
} ;

static const sigInfo sigArray[] =
{
#ifdef SIGINT
	{ SIGINT },
#endif
#ifdef SIGHUP
	{ SIGHUP },
#endif
#ifdef SIGPIPE
	{ SIGPIPE },
#endif
#ifdef SIGTERM
	{ SIGTERM },
#endif
#ifdef SIGPOLL
	{ SIGPOLL },
#endif
#ifdef SIGUSR1
	{ SIGUSR1 },
#endif
#ifdef SIGUSR2
	{ SIGUSR2 },
#endif
	{ -31337 }
} ;

// Includes the -31337 element
size_t signalCount ;

void	handleChild() ;
void	handleParent( pid_t childPID, int numSignals ) ;
void	setupSignals() ;
int	randomSignal() ;

int main( int argc, char** argv )
{
if( argc != 2 )
	{
	cout	<< "Usage: "
		<< argv[ 0 ]
		<< " <num signals>"
		<< endl ;
	return 0 ;
	}

elog.setStream( &std::clog ) ;

::srand( ::time( 0 ) ) ;
setupSignals() ;

cout	<< "Parent PID: "
	<< getpid()
	<< endl ;

int numSignals = ::atoi( argv[ 1 ] ) ;

// Fork a child
pid_t thePID = fork() ;
if( 0 == thePID )
	{
	clog	<< "Child PID: "
		<< getpid()
		<< endl ;

	handleChild() ;

	clog	<< "Child exiting..."
		<< endl ;

	::exit( 0 ) ;
	}
else
	{
	// Parent
	sleep( 2 ) ;

	handleParent( thePID, numSignals ) ;

	clog	<< "Parent exiting..."
		<< endl ;

	::exit( 0 ) ;
	}
return 0 ;
}

void handleChild()
{
// Instantiate the singleton, which will configure itself to
// catch all signals
Signal::getInstance() ;

size_t totalSignalsCaught = 0 ;
while( true )
	{
	::sleep( 1 ) ;

	int theSignal = -1 ;
	while( Signal::getSignal( theSignal ) )
		{
		if( theSignal < 0 )
			{
			// Error
			clog	<< "handleChild> Error"
				<< endl ;
			continue ;
			}

		++totalSignalsCaught ;
		clog	<< "Child> Got signal: "
			<< theSignal
			<< ", total signals caught: "
			<< totalSignalsCaught
			<< endl ;
		}
	}

clog	<< "Child> Caught "
	<< totalSignalsCaught
	<< " signals"
	<< endl ;

} // handleChild()

void handleParent( pid_t childPID, int numSignals )
{
size_t totalSignalsSent = 0 ;

for( ; totalSignalsSent < static_cast< size_t >( numSignals ) ; )
	{
	for( size_t i = 0 ; i < (signalCount - 1) ; ++i )
		{
		++totalSignalsSent ;
		clog	<< "Parent> Sending signal: "
			<< sigArray[ i ].signalID
			<< endl ;
		::kill( childPID, sigArray[ i ].signalID ) ;
		}
	sleep( 2 ) ;
	}

/*
for( int i = 0 ; i < numSignals ; ++i )
	{
	int theSig = randomSignal() ;
	clog	<< "Parent> Sending signal "
		<< theSig
		<< endl ;

	kill( childPID, theSig ) ;
	} // for( i )
*/
clog	<< "Parent> Sent "
	<< totalSignalsSent
	<< " signals"
	<< endl ;

sleep( 10 ) ;

//::kill( childPID, SIGKILL ) ;
}

void setupSignals()
{
// Count the number of signals
for( size_t i = 0 ; sigArray[ i ].signalID != -31337 ;
	++i, ++signalCount )
	{
	}
}

int randomSignal()
{
return (sigArray[ rand() % signalCount ].signalID) ;
}
