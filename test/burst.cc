/**
 * burst.cc
 */

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>

#include	<string>
#include	<iostream>
#include	<fstream>

#include	<cassert>
#include	<cstring>
#include	<cerrno>

#include	"gnuworld_config.h"

#ifdef HAVE_GETOPT_H
#include	<getopt.h>
#endif

#include	"ConnectionManager.h"
#include	"ConnectionHandler.h"
#include	"Connection.h"
#include	"ELog.h"

using namespace std ;
using namespace gnuworld ;

class localHandler : public ConnectionHandler
{
public:
	Connection*		listenPtr ;
	Connection*		burstPtr ;

	localHandler()
	: listenPtr( 0 ),
	  burstPtr( 0 )
	{}
	virtual ~localHandler()
	{}

	virtual void OnDisconnect( Connection* cPtr )
	{
	if( cPtr == burstPtr )
		{
		burstPtr = 0 ;
		}
	}

	virtual void OnConnect( Connection* cPtr )
	{
	burstPtr = cPtr ;
	cout	<< "OnConnect> Received new connection: "
		<< *cPtr
		<< endl ;
	}
} ;

void usage( const string& progName )
{
cout	<< endl
	<< "Usage: "
	<< progName
	<< " [options]"
	<< endl ;
cout	<< endl
	<< "Options: "
	<< endl
	<< "-b <KB per second>\tNumber of kilobytes per second to "
	<< "transfer"
	<< endl
	<< "-f <socket filename>\tThe socket file name (required)"
	<< endl
	<< "-p <port>\t\tPort on which to listen"
	<< endl
	<< "-h\t\t\tPrint this help menu"
	<< endl ;
cout	<< endl ;
}

int main( int argc, char** argv )
{
if( 1 == argc )
	{
	usage( argv[ 0 ] ) ;
	return 0 ;
	}

elog.setStream( &cout ) ;

size_t numKB = 250 ;
unsigned short int portNum = 45678 ;
string socketFileName ;

int c = EOF ;
while( (c = ::getopt( argc, argv, "f:hp:b:" )) != EOF )
	{
	switch( c )
		{
		case 'b':
			numKB = static_cast< size_t >( atoi( optarg ) ) ;
			break ;
		case 'f':
			socketFileName = optarg ;
			break ;
		case 'h':
			usage( argv[ 0 ] ) ;
			return 0 ;
		case 'p':
			portNum = static_cast< unsigned short int >(
				atoi( optarg ) ) ;
			break ;
		default:
			cout	<< "Unknown option: "
				<< (char) c
				<< endl ;
			return -1 ;
		} // switch()
	} // while( getopt() )

ConnectionManager cm ;
localHandler lh ;

lh.listenPtr = cm.Listen( &lh, portNum ) ;
if( 0 == lh.listenPtr )
	{
	cout	<< "Unable to establish listener on port "
		<< portNum
		<< endl ;
	return -1 ;
	}

int inputFileFD = ::open( socketFileName.c_str(), O_RDONLY ) ;
if( inputFileFD < 0 )
	{
	cout	<< "Unable to open input file: "
		<< socketFileName
		<< ": "
		<< strerror( errno )
		<< endl ;
	cm.Disconnect( &lh, lh.listenPtr ) ;
	return -1 ;
	}

cout	<< endl
	<< "Listening for incoming connections on port "
	<< portNum
	<< ", processing from burst file: "
	<< socketFileName
	<< "..."
	<< endl ;

size_t totalReadBytes = 0 ;
size_t attemptReadSize = numKB * 1024 ;
char* inputBuffer = new (std::nothrow) char[ attemptReadSize ] ;
assert( inputBuffer != 0 ) ;

bool done = false ;
while( !done )
	{
	if( 0 == lh.burstPtr )
		{
		// No connection yet
		// Let the CM check for new connection,
		// block indefinitely.
		cm.Poll( -1 ) ;
		}

	if( 0 == lh.burstPtr )
		{
		// Timed out or event occured
		continue ;
		}

	// Poll(), blocking for up to 1 second
	cm.Poll( 1, 0 ) ;

	if( lh.burstPtr->getOutputBufferSize() >=
		(4 * attemptReadSize) )
		{
		// Don't overfill the output buffer
		continue ;
		}

	time_t connectDuration = ::time( 0 ) - lh.burstPtr->getConnectTime() ;
	if( 0 == connectDuration )
		{
		connectDuration = 1 ;
		}
	size_t recvRate = lh.burstPtr->getBytesRead() / connectDuration ;
	size_t sendRate = lh.burstPtr->getBytesWritten()
		/ connectDuration ;

	cout	<< "Duration: "
		<< connectDuration
		<< " seconds; Read: "
		<< lh.burstPtr->getBytesRead()
		<< " bytes total, "
		<< (recvRate >> 10)
		<< " KB/sec; Write: "
		<< lh.burstPtr->getBytesWritten()
		<< " bytes total, "
		<< (sendRate >> 10)
		<< " KB/sec"
		<< endl ;

	// Connection exists, pass it some input
	int readBytes = ::read( inputFileFD, inputBuffer, attemptReadSize ) ;
	if( readBytes <= 0 )
		{
		cout	<< "Done reading input file "
			<< socketFileName.c_str()
			<< endl ;
		done = true ;
		continue ;
		}

	totalReadBytes += readBytes ;

	string writeMe( inputBuffer, readBytes ) ;
	lh.burstPtr->Write( writeMe ) ;

//	cout	<< "main> Writing "
//		<< readBytes
//		<< " bytes"
//		<< endl ;

	} // while( !done )


delete[] inputBuffer ; inputBuffer = 0 ;
::close( inputFileFD ) ;
return 0 ;
}
