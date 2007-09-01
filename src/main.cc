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
 * $Id: main.cc,v 1.67 2007/09/01 20:16:56 mrbean_ Exp $
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

#include	"gnuworld_config.h"

/*
#ifdef HAVE_GETOPT_H
#include	<getopt.h>
#endif
*/

#include	"ELog.h"
#include	"server.h"
#include	"moduleLoader.h"
#include	"md5hash.h"
#include	"Signal.h"

#ifdef ENABLE_LOG4CPLUS
#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#endif

RCSTAG( "$Id: main.cc,v 1.67 2007/09/01 20:16:56 mrbean_ Exp $" ) ;

// main() must be in the global namespace
using namespace gnuworld ;

using std::cerr ;
using std::clog ;
using std::endl ;
using std::string ;

// Output the GNU greeting
void		gnu() ;

/// Output the command line arguments for gnuworld
void usage( const string& progName )
{
clog	<< "Usage: " << progName << " [options]"
	<< endl ;
clog	<< endl
	<< "Options:"
	<< endl ;
clog	<< "  -c\t\t\tVerbose output"
	<< endl ;
clog	<< "  -d <debug filename>\tSpecify the debug output file"
	<< endl ;
clog	<< "  -D\t\t\tDisable debug logging"
	<< endl ;
clog	<< "  -f <conf filename>\tSpecify the config file name"
	<< endl ;
clog	<< "  -h\t\t\tPrint this help menu" 
	<< endl ;
clog	<< "  -l <log file>\t\tCapture raw socket data to log file"
	<< endl ; 
clog	<< "  -L\t\t\tDisable logging of socket data to file"
	<< endl ;
clog	<< "  -s <socket file>\tRun in simulation mode"
	<< endl ;
clog	<< endl ;
}

/// Output the GNUWorld/GNU welcome message
void gnu()
{
clog	<< endl ;
clog	<< "GNUWorld version " << VERSION << endl ;
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

#ifdef DAEMON
	/* fork into background here */
	if (fork())
	{
		fprintf(stdout, "%s: forked into background\n", argv[0]);
		exit(0);
	}
	setsid();
	if (fork())
		exit(0);
	/* redirect stdin, stdout, stderr to null */
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);
#endif

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
	std::ofstream pidFile( "gnuworld.pid", std::ios::trunc | 
				std::ios::out ) ;
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
   elogFileName( "debug.log" ),
   configFileName( CONFFILE )
{
logSocket = true ;
verbose = false ;
bool doDebug = true ;
string socketFileName( "socket.log" ) ;

std::cout << "Before configuration" << endl; 
#ifdef ENABLE_LOG4CPLUS
log4cplus::PropertyConfigurator::doConfigure("logging.properties");
#endif

optind = 0 ;
int c = EOF ;
while( (c = getopt( argc, argv, "cd:Df:l:Lhs:")) != -1 )
	{
	switch( c )
		{
		case 0:
		case 'c':
			verbose = true ;
			break ;
		case 1:
		case 'd':
			doDebug = true ;
			elogFileName = optarg ;
			break ;
		case 'D':
			doDebug = false ;
			break ;
		case 2:
		case 'f':
			configFileName = optarg ;
			break ;
		case 3:
		case 'h':
			usage( argv[ 0 ] ) ;
			::exit( 0 ) ;
		case 'l':
			logSocket = true ;
			socketFileName = optarg ;
			break ;
		case 'L':
			logSocket = false ;
			break ;
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

if( doDebug )
	{
	elog.openFile( elogFileName ) ;
	if( !elog.isOpen() )
		{
		clog	<< "*** Unable to open elog file: "
			<< elogFileName
			<< endl ;
		::exit( 0 ) ;
		}
	clog	<< "*** Running in debug mode..."
		<< endl ;
	}

if( verbose )
	{
	elog.setStream( &clog ) ;
	elog	<< "*** Running in verbose mode..."
		<< endl ;
	}

if( logSocket )
	{
	socketFile.open( socketFileName.c_str(), std::ios::out ) ;
	if( !socketFile.is_open() )
		{
		clog	<< "*** Unable to open socket log file: "
			<< socketFileName
			<< endl ;
		::exit( -1 ) ;
		}
	clog	<< "*** Logging raw data to "
		<< socketFileName
		<< "..."
		<< endl ;
	}

// Sets up the server internals
initializeSystem() ;
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
	// Check if a reconnection is necessary
	// Do not reconnect if the server is in the process of
	// shutting down.
	if( !isConnected() && !isLastLoop() )
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

	// Check if this is the last iteration of the main processing
	// loop.  If so, set keepRunning to false.
	// The server Shutdown() (setting lastLoop to true) can occur
	// in the timers or during the processing of network
	// connections (Poll()), or possibly in a signal handler.
	// That means that is lastLoop is already true at this point,
	// then it was set thus on the last iteration, and this is indeed
	// the final iteration of the processing loop.
	if( isLastLoop() )
		{
		keepRunning = false ;

		// All output for the xClients should be in the
		// output buffer by now, go ahead and put the
		// server's SQ message there as well
		Write( "%s SQ %s :%s",
			getCharYY().c_str(),
			getCharYY().c_str(),
			getShutDownReason().c_str() ) ;

		// Make sure the SQ and all previous data are flushed
		// to the network before disconnecting.
		FlushData() ;
		}

	// Check for pending timers before calling Poll()
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
