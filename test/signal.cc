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
 */

#include	<sys/types.h>
#include	<unistd.h>

#include	<fstream>
#include	<iostream>

#include	<cerrno>
#include	<cstring>

#include	"Signal.h"
#include	"ELog.h"

using namespace std ;
using namespace gnuworld ;

ELog gnuworld::elog ;

/// Subclass class Signal to introduce a method which will
/// force some more interesting situations to occur
class mySignal : public Signal
{
public:
	void	forceLock()
		{ ::pthread_mutex_lock( &signals1Mutex ) ; }
	void	forceUnlock()
		{ ::pthread_mutex_unlock( &signals1Mutex ) ; }



} ;

int main()
{

cout	<< "PID: "
	<< getpid()
	<< endl ;

ofstream outFile( "signal.pid" ) ;
if( !outFile )
	{
	cout	<< "Error: Unable to open file: signal.pid"
		<< strerror( errno )
		<< endl ;
	return 0 ;
	}

outFile	<< getpid() << endl ;
outFile.close() ;

// Force registration of signal handlers
mySignal sig ;

// Force the signals1Mutex to lock
// This should force placement of signals into signals2
cout	<< "Locking signals1Mutex" << endl ;
sig.forceLock() ;

cout	<< "Please issue signals now." << endl ;

// The user is expected to introduce signals to this process
// during this loop

// If the user hits CTRL-C (which he/she should be doing here),
// the sleep() function will return before completion, returning
// the number of seconds left to sleep.
// Continue until an entire 10 seconds goes by without receiving
// any signals.
while( sleep( 10 ) != 0 )
	{
	}

sig.forceUnlock() ;

cout	<< "Sleeping without lock, please issue signals." << endl ;
while( sleep( 10 ) != 0 )
	{
	}

int theSig = -1 ;
while( (theSig = sig.getSignal()) != -1 )
	{
	cout	<< "Retrieved signal: "
		<< theSig
		<< endl ;
	}

return 0 ;
}

