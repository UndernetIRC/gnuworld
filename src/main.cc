/* main.cc
 * $Id: main.cc,v 1.5 2000/07/31 16:33:50 dan_karrels Exp $
 */

#include	<fstream>
#include	<new>

#include	<cstdio>
#include	<cstdlib>
#include	<cstring>
#include	<csignal> 
#include	<dlfcn.h>

#include	"config.h"
#include	"ELog.h"
#include	"Network.h"
//#include	"ccontrol.h"
#include	"cloner.h"
#include	"clientExample.h"
#include	"stats.h"
#include	"FileSocket.h" 
#include	"moduleLoader.h"

#ifdef LOG_SOCKET
	std::ofstream socketFile ;
#endif

using namespace gnuworld ;

const char config_h_rcsId[] = __CONFIG_H ;
const char main_cc_rcsId[] = "$Id: main.cc,v 1.5 2000/07/31 16:33:50 dan_karrels Exp $" ;

using std::cerr ;
using std::clog ;
using std::endl ;
using std::string ;
 
gnuworld::xNetwork*	gnuworld::Network ;
gnuworld::ELog		gnuworld::elog ;
gnuworld::xServer*	Server ;

volatile bool keepRunning = true ;

void sigHandler( int whichSig )
{
switch( whichSig )
	{
	case SIGUSR1:
		if( Server )
			{
			Server->dumpStats() ;
			}
		break ;
	default:
		elog << "*** Shutting down...\n" ;
		keepRunning = false ;
		break ;
	}
}

void usage( const string& progName )
{
clog << "Usage: " << progName << " [options]\n" ;
clog << "\nOptions:\n" ;
clog << "-c: Verbose output\n" ;
#ifdef EDEBUG
  clog << "-d <debug filename>: Specify the debug output file\n" ;
#endif
clog << "-f <conf filename>: Specify the config file name\n" ;
clog << "-h: Print this help menu\n" ;
clog << "-s <socket file>: Run in simulation mode\n" ;
clog << endl ;
}

int main( int argc, char** argv )
{

string simFileName ;
string confFileName( CONFFILE ) ;

#ifdef EDEBUG
  string elogFileName( DEBUGFILE ) ;
#endif

bool verbose = false ;

int c = EOF ;
while( (c = getopt( argc, argv, "cd:f:hs:" )) != EOF )
	{
	switch( c )
		{
		case 'c':
			verbose = true ;
			break ;
#ifdef EDEBUG
		case 'd':
			elogFileName = optarg ;
			break ;
#endif
		case 'f':
			confFileName = optarg ;
			break ;
		case 'h':
			usage( argv[ 0 ] ) ;
			return 0 ;
		case 's':
			simFileName = optarg ;
			clog << "*** Running in simulation mode...\n" ;
			break ;
		case ':':
			clog << "*** Missing parameter\n" ;
			usage( argv[ 0 ] ) ;
			return 0 ;
		case '?':
			break ;
		default:
			clog << "Unknown option " << (char) c << endl ;
			usage( argv[ 0 ] ) ;
			return 0 ;
		} // close switch
	} // close while

if( SIG_ERR == ::signal( SIGINT, sigHandler ) )
	{
	clog	<< "*** Unable to establish signal hander for SIGINT\n" ;
	return 0 ;
	}
if( SIG_ERR == ::signal( SIGUSR1, sigHandler ) )
	{
	clog	<< "*** Unable to establish signal hander for SIGUSR1\n" ;
	return 0 ;
	}

#ifdef EDEBUG
	elog.openFile( elogFileName.c_str() ) ;
	if( !elog.isOpen() )
		{
		clog	<< "*** Unable to open elog file: " << elogFileName
			<< endl ;
		return 0 ;
		}
	clog	<< "*** Running in debug mode...\n" ;
#endif

if( verbose )
	{
	elog.setStream( &clog ) ;
	elog	<< "*** Running in verbose mode...\n" ;
	}

#ifdef LOG_SOCKET
	socketFile.open( LOG_SOCKET_NAME, std::ios::out ) ;
	if( !socketFile.is_open() )
		{
		clog	<< "*** Unable to open socket log file: "
			<< LOG_SOCKET_NAME << endl ;
		return 0 ;
		}
#endif

srand( time( 0 ) ) ;

char		s[ 4096 ] = { 0 } ;

try
	{
	Network = new xNetwork ;

	// Instantiate the server object
	Server = new xServer( confFileName ) ;

	// Run in simulation mode?
	if( !simFileName.empty() )
		{
		Server->setSocket( new FileSocket( simFileName ) ) ;
		}

	}
catch( std::bad_alloc )
	{
	cerr	<< "*** Memory allocation failure\n" ;
	return -1 ;
	}

// Instantiate and attach clients to the server
// TODO: Have all clients loaded from modules
// --> Easier client manipulation at runtime
// NOTE: This is a memory leak, *shrug*
//Server->AttachClient( new ccontrol( "gnuworld3.conf" ) ) ;
//Server->AttachClient( new cloner( "cloner.conf" ) ) ;
//Server->AttachClient( new stats( "stats.conf" ) ) ;
Server->AttachClient( new clientExample( "clientExample.conf" ) ) ;
 
// Connect to the server
clog << "*** Connecting...\n" ;

if( Server->Connect() < 0 )
	{
	clog << "*** Cannot get connected to server!\n" ;
#ifdef EDEBUG
	elog.closeFile() ;
#endif
	return -1 ;
	}

clog	<< "*** Connection Established!\n" ;

// The number of messages processed during any one loop
size_t messages = 0 ;

// Main server loop
while( keepRunning )
	{

	// Is the server connected?
	if( !Server->isConnected() )
		{
		elog	<< "*** Not connected, aborting\n" ;
		keepRunning = false ;
		continue ;
		}

	// Try to read from the network.
	if( Server->ReadyForRead() )
		{
		// Data available, let's read it
		if( Server->DoRead() < 0 )
			{
			elog << "*** Read error!\n" ;
			keepRunning = false ;
			continue ;
			}
		}

	// Process any new data.
	while( Server->GetString( s, sizeof( s ) ) )
		{
		if( s[ 0 ] != 0 )
			{
#ifdef LOG_SOCKET
			socketFile << s << endl ;
#endif
			if( verbose )
				{
				clog << "[IN]:  " << s << endl ;
				}

			// handle the new data
			Server->Process( s ) ;

			// Count the number of messages processed
			messages++ ;
			}
		} // while( Server->GetString() )

	// Are we ready to perform a write operation?
	if( Server->ReadyForWrite() )
		{
		if( !Server->flushBuffer() )
			{
			elog << "*** Write error!\n" ;
			keepRunning = false ;
			continue ;
			}
		}

	if( Server->GetMessage() != SRV_SUCCESS )
		{
		elog	<< "*** Received message "
			<< Server->GetMessage()
			<< ", shutting down...\n" ;
		keepRunning = false ;
		continue ;
		}

	// Did we process any messages?
	if( 0 == messages )
		{
		// No data handled, pause
		::usleep( 1000 ) ;
		}

	// Else, data was read and handled, no sleep
	messages = 0 ;

	} // close while( keepRunning )

//delete Server ;
//delete Network ;

#ifdef EDEBUG
	elog.closeFile() ;
#endif

#ifdef LOG_SOCKET
	socketFile.close() ;
#endif

return 0 ;

}
