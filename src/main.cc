/**
 * main.cc
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
 * $Id: main.cc,v 1.56 2003/08/06 17:47:48 dan_karrels Exp $
 */

#include	<sys/time.h>
#include	<sys/types.h>
#include	<unistd.h>

#include	<new>
#include	<fstream>
#include	<iostream>

#include	<cstdio>
#include	<cassert>
#include	<cstdlib>
#include	<cstring>

#include	"config.h"

#ifdef HAVE_GETOPT_H
#include	<getopt.h>
#endif

#include	"ELog.h"
#include	"server.h"
#include	"moduleLoader.h"
#include	"md5hash.h"
#include	"Signal.h"

RCSTAG( "$Id: main.cc,v 1.56 2003/08/06 17:47:48 dan_karrels Exp $" ) ;

// main() must be in the global namespace
using namespace gnuworld ;

using std::cerr ;
using std::clog ;
using std::endl ;
using std::string ;

// Output the GNU greeting
void		gnu() ;

// Instantiate the logging stream
gnuworld::ELog		gnuworld::elog ;

/// Output the command line arguments for gnuworld
void usage( const string& progName )
{
clog << "Usage: " << progName << " [options]\n" ;
clog << "\nOptions:\n" ;
clog << "  -c, --verbose\t\tVerbose output\n" ;
#ifdef EDEBUG
  clog << "  -d <debug filename>, --debug=<debug file name>" << endl ;
  clog << "\t\t\tSpecify the debug output file\n" ;
#endif
clog << "  -f <conf filename>, --config=<config filename>" << endl ;
clog << "\t\t\tSpecify the config file name\n" ;
clog << "  -h, --help\t\tPrint this help menu\n" ;
clog << "  -s <socket file>, --socket=<socket file name>" << endl ;
clog << "\t\t\tRun in simulation mode\n" ;
clog << endl ;
}

/// Output the GNUWorld/GNU welcome message
void gnu()
{
clog	<< "GNUWorld version 2.2" << endl ;
clog	<< "Copyright (C) 2002 Free Software Foundation, Inc." << endl ;
clog	<< "GNUWorld comes with NO WARRANTY," << endl ;
clog	<< "to the extent permitted by law." << endl ;
clog	<< "You may redistribute copies of GNUWorld" << endl ;
clog	<< "under the terms of the GNU General Public License." << endl ;
clog	<< "For more information about these matters," << endl ;
clog	<< "see the files named COPYING." << endl ;
clog	<< endl ;
}

// These functions are used to duplicate/free
// argv for reconnection of the server.
// For some reason gcc 3.3 won't let me declare
// argv as const char**
char** dupArray( int argc, char** argv )
{
assert( argc >= 1 ) ;

// Put a trailing NULL as the last element of the array
char** retMe = new char*[ argc + 1 ] ;
for( int i = 0 ; i < argc ; ++i )
	{
	retMe[ i ] = new char[ strlen( argv[ i ] ) + 1 ] ;
	strcpy( retMe[ i ], argv[ i ] ) ;
	}
retMe[ argc ] = 0 ;
return retMe ;
}

void releaseDup( int argc, char** releaseMe )
{
assert( argc >= 1 ) ;
assert( releaseMe != 0 ) ;

for( int i = 0 ; i < argc ; ++i )
	{
	delete[] releaseMe[ i ] ;
	}
delete[] releaseMe ;
}

int main( int argc, char** argv )
{
// output gnu information
gnu() ;

// This is done to intialize the hasher
md5 dummy ;

// Load signal handlers.  This need only be done once
// for the entire system, no matter if the xServer is reloaded.
if( 0 == Signal::getInstance() )
	{
	clog	<< "Unable to initialize signal handlers"
		<< endl ;
	return -1 ;
	}

// Seed the random number generator
::srand( ::time( 0 ) ) ;

bool autoConnect = true ;
while( autoConnect )
	{
	// getopt() mutates argv, so only pass a copy so
	// we can call getopt() again with the same option set
	char** dupArgv = dupArray( argc, argv ) ;

	// Allocate a new instance of the xServer
	gnuworld::xServer* theServer =
		new (std::nothrow) gnuworld::xServer( argc, dupArgv ) ;
	assert( theServer != 0 ) ;

	// Write out the pid
	// TODO: This will have to be updated when running
	// in background.
	{
	ofstream pidFile( "gnuworld.pid", ios::trunc | ios::out ) ;
	if( !pidFile )
		{
		clog	<< "Unable to open pid file: gnuworld.pid"
			<< endl ;

		delete theServer ; theServer = 0 ;
		return -1 ;
		}

	pidFile	<< getpid()
		<< endl ;

	pidFile.close() ;
	} // pidFile

	theServer->run() ;

	// update autoConnect here so we can tell how the server
	// terminated.
	autoConnect = theServer->getAutoConnect() ;

	delete theServer ; theServer = 0 ;

	releaseDup( argc, dupArgv ) ;
	dupArgv = 0 ;

	::sleep( 10 ) ;
	} // while( autoConnect )

return 0 ;
}

xServer::xServer( int argc, char** argv )
 : eventList( EVT_NOOP ),
#ifdef EDEBUG
   elogFileName( DEBUGFILE ),
#endif
   configFileName( CONFFILE )
{
verbose = false ;

optind = 0 ;
int c = EOF ;
while( (c = getopt( argc, argv, "cd:f:hs:")) != EOF )
//while( true )
	{
/*
	int option_index = 0 ;
	struct option cmdLineArgs[] = {
		{ "verbose", no_argument, NULL, 0 },
		{ "debug", no_argument, NULL, 1 },
		{ "config", required_argument, NULL, 2 },
		{ "help", no_argument, NULL, 3 },
		{ "socket", required_argument, NULL, 4 },
		{ 0, 0, 0, 0 }
	} ;
	c = getopt_long_only( argc, argv, "cd:f:hs:",
		cmdLineArgs, &option_index ) ;
	if( -1 == c )
		{
		break ;
		}
*/
	switch( c )
		{
		case 0:
		case 'c':
			verbose = true ;
			break ;
#ifdef EDEBUG
		case 1:
		case 'd':
			elogFileName = optarg ;
			break ;
#endif
		case 2:
		case 'f':
			configFileName = optarg ;
			break ;
		case 3:
		case 'h':
			usage( argv[ 0 ] ) ;
			::exit( 0 ) ;
		case 4:
		case 's':
			simFileName = optarg ;
			clog	<< "*** Running in simulation mode..."
				<< endl ;
			break ;
		case ':':
			clog << "*** Missing parameter\n" ;
			usage( argv[ 0 ] ) ;
			::exit( 0 ) ;
		case '?':
			break ;
		default:
			clog << "Unknown option " << (char) c << endl ;
			usage( argv[ 0 ] ) ;
			::exit( 0 ) ;
		} // close switch
	} // close while

#ifdef EDEBUG
	elog.openFile( elogFileName.c_str() ) ;
	if( !elog.isOpen() )
		{
		clog	<< "*** Unable to open elog file: "
			<< elogFileName
			<< endl ;
		::exit( 0 ) ;
		}
	clog	<< "*** Running in debug mode..."
		<< endl ;
#endif

if( verbose )
	{
	elog.setStream( &clog ) ;
	elog	<< "*** Running in verbose mode..."
		<< endl ;
	}

// Sets up the server internals
initializeSystem() ;

#ifdef LOG_SOCKET
	socketFile.open( LOG_SOCKET_NAME, std::ios::out ) ;
	if( !socketFile.is_open() )
		{
		clog	<< "*** Unable to open socket log file: "
			<< LOG_SOCKET_NAME
			<< endl ;
		::exit( -1 ) ;
		}
#endif
}

void xServer::mainLoop()
{
// If this variable is true, check for a signal
// Otherwise, there was a critical failure in the signal
// subsystem, and do not check for new signals.
bool checkSignals = true ;

// When this method is first invoked, the server is not connected
while( keepRunning )
	{
	if( NULL == serverConnection )
		{
		// Connect to the server/file
		clog	<< "*** Connecting " ;

		// Not connected
		if( !simFileName.empty() )
			{
			// Run in simulation mode
			clog	<< "to file "
				<< simFileName
				<< "... "
				<< endl ;

			serverConnection = ConnectToFile( this, simFileName ) ;
			if( NULL == serverConnection )
				{
				// Failed to connect to file, quit
				keepRunning = false ;

				continue ;
				}
			}
		else
			{
			// Run in real mode
			clog	<< "to "
				<< UplinkName
				<< "... "
				<< endl ;

			serverConnection = Connect( this, UplinkName, Port ) ;
			}
		} // if( NULL == serverConnection )

	// Check for pending timers before calling Poll(), just for
	// If timers are pending, then set the duration until the next
	// arriving timer expiration to be the maximum wait time
	// for Poll()

	// If there are timers waiting, this variable will be set to
	// true.  A value of false means that no timers are pending,
	// and that Poll() can block indefinitely.
	bool setTimers = false ;

	long seconds = -1 ;
	time_t now = ::time( 0 ) ;

        if( !timerQueue.empty() )
                {
//		elog	<< "mainLoop> Found a timer"
//			<< endl ;

                // Yes, set select() timeout to time at which
                // the nearest timer will expire
                setTimers = true ;

		// Set tv.tv_sec to the duration (in seconds) until
		// the first timer will expire.
                seconds = timerQueue.top().second->absTime - now ;

		if( seconds < 0 )
			{
			// A negative value here will result
			// in inifinite blocking in Poll(),
			// which is exactly opposite of what is needed here
			seconds = 0 ;
			}
                } // if( !timerQueue.empty() )

	// Process all available data
	ConnectionManager::Poll( seconds ) ;

	// Poll() will call all appropriate data handlers
	// Check the timers
	CheckTimers() ;

	if( !checkSignals )
		{
		// There was a critical failure in the Signal handling
		// system, do not check for new signals.
		continue ;
		}

        // Did we catch a signal?
	int theSignal = -1 ;
	while( Signal::getSignal( theSignal ) )
		{
		if( -1 == theSignal )
			{
			elog	<< "xServer::mainLoop> Criticial failure "
				<< "in the signal handling system"
				<< endl ;
			checkSignals = false ;
			break ;
			}

		elog	<< "xServer::mainLoop> Received signal: "
			<< theSignal
			<< endl ;

		if( !PostSignal( theSignal ) )
			{
			keepRunning = false ;
			// No need to set checkSignals to false here,
			// since this big while() loop will not be
			// executed again.
			break ;
			}
                } // while( Signal::getSignal() )

	} // while( keepRunning )

// We are shutting down
doShutdown() ;

} // mainLoop()
