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
 * $Id: main.cc,v 1.49 2002/08/07 21:11:11 dan_karrels Exp $
 */

#include	<new>
#include	<fstream>
#include	<iostream>

#include	<sys/time.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<getopt.h>

#include	<cstdio>
#include	<cstdlib>
#include	<cstring>
#include	<csignal> 

#include	"config.h"
#include	"ELog.h"
#include	"server.h"
#include	"moduleLoader.h"
#include	"md5hash.h"

const char config_h_rcsId[] = __CONFIG_H ;
const char main_cc_rcsId[] = "$Id: main.cc,v 1.49 2002/08/07 21:11:11 dan_karrels Exp $" ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char server_h_rcsId[] = __SERVER_H ;
const char moduleLoader_h_rcsId[] = __MODULELOADER_H ;

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

/// This method is called when a registered system signal occurs
void xServer::sigHandler( int whichSig )
{
xServer::caughtSignal = true ;
xServer::whichSig = whichSig ;
}

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
clog	<< "GNUWorld version 2.1" << endl ;
clog	<< "Copyright (C) 2002 Free Software Foundation, Inc." << endl ;
clog	<< "GNUWorld comes with NO WARRANTY," << endl ;
clog	<< "to the extent permitted by law." << endl ;
clog	<< "You may redistribute copies of GNUWorld" << endl ;
clog	<< "under the terms of the GNU General Public License." << endl ;
clog	<< "For more information about these matters," << endl ;
clog	<< "see the files named COPYING." << endl ;
clog	<< endl ;
}

int main( int argc, char** argv )
{

// output gnu information
gnu() ;

// This is done to intialize the hasher
md5 dummy ;

// Allocate a new instance of the xServer
gnuworld::xServer* theServer =
	new (std::nothrow) gnuworld::xServer( argc, argv ) ;
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
	}

theServer->run() ;
delete theServer ;
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

if( !setupSignals() )
	{
	clog	<< "Failed to establish signal handlers"
		<< endl ;
	::exit( -1 ) ;
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

// Seed the random number generator
::srand( ::time( 0 ) ) ;
}

void xServer::mainLoop()
{
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

		}

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
                }

	// Process all available data
	ConnectionManager::Poll( seconds ) ;

	// Poll() will call all appropriate data handlers
	// Check the timers
	CheckTimers() ;

        // Did we catch a signal?
        if( caughtSignal )
                {
		if( !PostSignal( whichSig ) )
			{
			keepRunning = false ;
			continue ;
			}
		caughtSignal = false ;
		whichSig = 0 ;
                }

	} // while( keepRunning )
} // mainLoop()

bool xServer::setupSignals()
{
if( SIG_ERR == ::signal( SIGINT,
	static_cast< void (*)( int ) >( sigHandler ) ) )
	{
	clog	<< "*** Unable to establish signal hander for SIGINT"
		<< endl ;
	return false ;
	}
if( SIG_ERR == ::signal( SIGPIPE,
	static_cast< void (*)( int ) >( sigHandler ) ) )
	{
	clog	<< "*** Unable to establish signal hander for SIGPIPE"
		<< endl ;
	return false ;
	}
if( SIG_ERR == ::signal( SIGUSR1,
	static_cast< void (*)( int ) >( sigHandler ) ) )
	{
	clog	<< "*** Unable to establish signal hander for SIGUSR1"
		<< endl ;
	return false ;
	}
if( SIG_ERR == ::signal( SIGUSR2,
	static_cast< void (*)( int ) >( sigHandler ) ) )
	{
	clog	<< "*** Unable to establish signal hander for SIGUSR2"
		<< endl ;
	return false ;
	}

if( SIG_ERR == ::signal( SIGTERM,
	static_cast< void (*)( int ) >( sigHandler ) ) )
	{
	clog	<< "*** Unable to establish signal hander for SIGTERM"
		<< endl ;
	return false ;
	}
if( SIG_ERR == ::signal( SIGHUP,
	static_cast< void (*)( int ) >( sigHandler ) ) )
	{
	clog	<< "*** Unable to establish signal hander for SIGHUP"
		<< endl ;
	return false ;
	}
return true ;
}
