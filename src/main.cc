/* main.cc
 * $Id: main.cc,v 1.28 2001/03/01 03:26:41 dan_karrels Exp $
 */

#include	<new>
#include	<fstream>

#include	<sys/time.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<dlfcn.h>

#include	<cstdio>
#include	<cstdlib>
#include	<cstring>
#include	<csignal> 

#include	"config.h"
#include	"ELog.h"
#include	"FileSocket.h" 
#include	"server.h"
#include	"moduleLoader.h"

using namespace gnuworld ;

const char config_h_rcsId[] = __CONFIG_H ;
const char main_cc_rcsId[] = "$Id: main.cc,v 1.28 2001/03/01 03:26:41 dan_karrels Exp $" ;

using std::cerr ;
using std::clog ;
using std::endl ;
using std::string ;
 
gnuworld::ELog		gnuworld::elog ;

void xServer::sigHandler( int whichSig )
{
xServer::caughtSignal = true ;
xServer::whichSig = whichSig ;
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
gnuworld::xServer* theServer =
	new (nothrow) gnuworld::xServer( argc, argv ) ;
assert( theServer != 0 ) ;

	// Write out the pid
	// TODO: This will have to be updated when running
	// in background.
	{
	ofstream pidFile( "gnuworld.pid", ios::trunc ) ;
	if( !pidFile )
		{
		clog	<< "Unable to open pid file: gnuworld.pid"
			<< endl ;

		delete theServer ; theServer = 0 ;
		return 0 ;
		}

	pidFile	<< getpid()
		<< endl ;

	pidFile.close() ;
	}

// Connect to the server
clog << "*** Connecting...\n" ;

if( theServer->Connect() < 0 )
	{
	clog << "*** Cannot get connected to server!\n" ;
	::exit( 0 ) ;
	}
else
	{
	clog	<< "*** Connection Established!\n" ;
	theServer->run() ;
	}
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

string simFileName ;

verbose = false ;

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
			configFileName = optarg ;
			break ;
		case 'h':
			usage( argv[ 0 ] ) ;
			::exit( 0 ) ;
		case 's':
			simFileName = optarg ;
			clog << "*** Running in simulation mode...\n" ;
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
		clog	<< "*** Unable to open elog file: " << elogFileName
			<< endl ;
		::exit( 0 ) ;
		}
	clog	<< "*** Running in debug mode...\n" ;
#endif

if( verbose )
	{
	elog.setStream( &clog ) ;
	elog	<< "*** Running in verbose mode...\n" ;
	}

// Sets up the server internals
initializeSystem() ;

#ifdef LOG_SOCKET
	socketFile.open( LOG_SOCKET_NAME, std::ios::out ) ;
	if( !socketFile.is_open() )
		{
		clog	<< "*** Unable to open socket log file: "
			<< LOG_SOCKET_NAME << endl ;
		::exit( 0 ) ;
		}
#endif

srand( ::time( 0 ) ) ;

// Run in simulation mode?
if( !simFileName.empty() )
	{
	setSocket( new FileSocket( simFileName ) ) ;
	}
}

void xServer::mainLoop()
{

timeval tv = { 0, 0 } ;
timeval local = { 0, 0 } ;

fd_set readSet, writeSet ;
time_t now = 0 ;
int selectRet = -1 ;
int sockFD = theSock->getFD() ;
bool setTimer = false ;
unsigned int cnt = 0 ; 
char charBuf[ 1024 ] = { 0 } ;

while( keepRunning && _connected )
        {

        memset( charBuf, 0, 1024 ) ;

        // Did we catch a signal?
        // This needs to be put at the top of this loop.
        // If select() returns -1 because of a signal, this
        // loop continues without executing the rest of the loop.
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

        setTimer = false ;
        selectRet = -1 ;
        cnt = 0 ;
        now = ::time( 0 ) ;   

        FD_ZERO( &readSet ) ;
        FD_ZERO( &writeSet ) ;

	FD_SET( sockFD, &readSet ) ;

        // Does the output buffer have any data?
        if( !outputBuffer.empty() )
                {
                // Yes, test for write state on the socket
                FD_SET( sockFD, &writeSet ) ;
                }

        // Run any timers before calling select()
        CheckTimers() ;

	tv.tv_sec = 0 ;
	tv.tv_usec = 0 ;
                 
        // Are there are any timers remaining to run?
        if( !timerQueue.empty() )
                {
                // Yes, set select() timeout to time at which
                // the nearest timer will expire
                setTimer = true ;

		// Set tv.tv_sec to the duration (in seconds) until
		// the first timer will expire.
                tv.tv_sec = timerQueue.top().second->absTime - now ;
                }
        else
                {
                // No timers waiting to run
                setTimer = false ;
                }
                
        // Call select() until the max loop count is reached
        // or a successful return is obtained
        do
                {
                // Be sure to reset errno before calling select()
                errno = 0 ;
		local = tv ;
                selectRet = ::select( sockFD + 1, &readSet, &writeSet,
                        0, setTimer ? &local : 0 ) ;
                }
                // We are not expecting any signals, but one
                // may occur anyway
                while( (selectRet < 0) &&
			(EINTR == errno) &&
			(cnt++ <= 10) ) ;
                 
	// Did a timer expire?  Note that the above loop is
	// non-deterministic.  All timers are checked before
	// calling select(), however a timer may expire
	// before we get to call select().  If this is the
	// case, the timeval structure will contain an invalid
	// time, and errno will be set to EINVAL.

	// Check if a timer expired
	if( (0 == selectRet) || (EINVAL == selectRet) )
		{
                // select() timed out..timer has expired
                CheckTimers() ;
                continue ;
		}
	else if( selectRet < 0 )
		{
                // Caught some form of signal
                elog    << "xServer::mainLoop> select() returned error: "
                        << strerror( errno ) << endl ;
                continue ;
                }
                 
        // Otherwise, select() returned a value greater than 0
        // At least one socket descriptor has a ready state
                
        // Check if there is a read state available
        if( FD_ISSET( sockFD, &readSet ) )
                {
                // Yes, read all available data
                DoRead() ;
                
                // Are we still connected?
                if( !_connected )
                        {
                        // Nope, doh!
                        continue ;
                        }
                }
                
        // Attempt to process any new data
        while( GetString( charBuf ) )
                {
#ifdef LOG_SOCKET
		socketFile << charBuf << endl ;
#endif
		if( verbose )
			{
			clog	<< "[IN]:  " << charBuf
				<< endl ;
			}

                Process( charBuf ) ;
                }

        // Can we write to the socket without blocking?
        if( FD_ISSET( sockFD, &writeSet ) )
                {
                // Attempt to write output buffer to socket
                DoWrite() ;
                
                // Are we still connecte?
                if( !_connected )
                        {
                        // Nope, doh!
                        continue ;
                        }
                }
        
        } // while( keepRunning && _connected )
                 
}

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
