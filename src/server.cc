/**
 * server.cc
 * This is the implementation file for the xServer class.
 * This class is the entity which is the GNUWorld server
 * proper.  It manages network I/O, parsing and distributing
 * incoming messages, notifying attached clients of
 * system events, on, and on, and on.
 */

#include	<new>
#include	<string>
#include	<list>
#include	<vector>
#include	<algorithm>
#include	<strstream>
#include	<stack>
#include	<pair.h>

#include	<sys/time.h>
#include	<unistd.h>

#include	<cstdlib>
#include	<cstdio>
#include	<cstdarg>
#include	<cstring>
#include	<cassert>
#include	<cerrno>
#include	<csignal>

#include	"config.h"
#include	"misc.h"
#include	"events.h"
#include	"ip.h"

#include	"server.h"
#include	"Network.h"
#include	"iServer.h"
#include	"iClient.h"
#include	"EConfig.h"
#include	"Gline.h"

#include	"ELog.h"
#include	"Socket.h"
#include	"StringTokenizer.h"
#include	"xparameters.h"
#include	"moduleLoader.h"
#include	"ServerTimerHandlers.h"

const char xServer_h_rcsId[] = __XSERVER_H ;
const char xServer_cc_rcsId[] = "$Id: server.cc,v 1.77 2001/02/18 19:46:01 dan_karrels Exp $" ;

using std::string ;
using std::vector ;
using std::list ;
using std::endl ;
using std::ends ;
using std::strstream ;
using std::stack ;
using std::unary_function ;

namespace gnuworld
{

xNetwork*	Network = 0 ;

int xServer::whichSig = 0 ;
bool xServer::caughtSignal = false ;

// Allocate the static std::string in xServer representing
// all channels.
const string xServer::CHANNEL_ALL( "*" ) ;

// This is a unary function class that will handle notifying
// each xClient of a received signal
struct handleSignal : public xNetwork::fe_xClientBase
{
handleSignal( int _whichSig ) : whichSig( _whichSig ) {}

virtual void operator() ( xClient* theClient )
{
if( NULL == theClient ) return ;
theClient->OnSignal( whichSig ) ;
}
 
int     whichSig ;
} ;

void xServer::initializeSystem()
{
initializeVariables() ;

if( !readConfigFile( configFileName ) )
	{
	clog	<< "Error reading config file: "
		<< configFileName << endl ;
	::exit( 0 ) ;
	}

// Output the information to the console.
elog << "Numeric: " << intYY << endl ;
elog << "Max Clients (bogus): " << intXXX << endl ;
elog << "Uplink Name: " << UplinkName << endl ;  
elog << "Port: " << Port << endl ;
elog << "Server Name: " << ServerName << endl ;
elog << "Server Description: " << ServerDescription << endl ;

inttobase64( charYY, intYY, 2 ) ;
inttobase64( charXXX, intXXX, 3 ) ;

elog << "xServer::charYY> " << charYY << endl ;
elog << "xServer::charXXX> " << charXXX << endl ;
elog << "xServer::intYY> " << intYY << endl ;
elog << "xServer::intXXX> " << intXXX << endl ;

iServer* me = new (nothrow) iServer(
	0,
	getCharYYXXX(),
	ServerName,
	::time( 0 ),
	::time( 0 ),
	Version ) ;
assert( me != 0 ) ;

if( !Network->addServer( me ) )
	{
	elog	<< "xServer::initializeSystem> Failed to add "
		<< "(me) to the system tables"
		<< endl ;
	::exit( 0 ) ;
	}

loadCommandHandlers() ;

if( !loadModules( configFileName ) )
	{
	elog	<< "xServer> Failed in loading one or more modules\n" ;
	::exit( 0 ) ;
	}

registerServerTimers() ;

}

/**
 * Deallocate this xServer instance.
 */
xServer::~xServer()
{
// TODO: Delete all clients
// TODO: Delete all commands in command map
// TODO: Deallocate all timers
delete commandMap ;

// Deallocate all of the Glines
for( glineListType::iterator ptr = glineList.begin() ; ptr != glineList.end() ;
	++ptr )
	{
	delete *ptr ;
	}
glineList.clear() ;

// Deallocate all loaded modules/close dlm handles.
for( moduleListType::iterator ptr = moduleList.begin() ;
	ptr != moduleList.end() ; ++ptr )
	{
	delete *ptr ;
	}
moduleList.clear() ;

while( !timerQueue.empty() )
	{
	delete timerQueue.top().second ;
	timerQueue.pop() ;
	}

#ifdef EDEBUG
	elog.closeFile()  ;
#endif

#ifdef LOG_SOCKET
	socketFile.close() ;
#endif

delete[] inputCharBuffer ; inputCharBuffer = 0 ;

} // ~xServer()

void xServer::loadCommandHandlers()
{

// Allocate the command map
commandMap = new (nothrow) commandMapType ;
assert( commandMap != 0 ) ;

// Register messages
// This basically adds command handler into
// the commandMap for each of the messages listed below.
// The format of REGISTER_MSG is:
// REGISTER_MSG(textCommand, handler)
// textCommand is the quotation delimited string
// associated with the incoming network command.
// handler is the name of the xServer non-static
// class handler method to be called for handling
// that particular command, prepended with MSG_

REGISTER_MSG( "ERROR", Error );
REGISTER_MSG( "RPING", RemPing );

// Server
REGISTER_MSG( "S", S );
REGISTER_MSG( "SERVER", Server );

// Nick
REGISTER_MSG( "N", N );

// End of Burst
REGISTER_MSG( "EB", EB );

// End of Burst Acknowledge
REGISTER_MSG( "EA", EA );

// Ping
REGISTER_MSG( "G", G );

// Privmsg
// Also register NOTICE to
// be handled by the privmsg
// command handler.
//
REGISTER_MSG( "O", P );
REGISTER_MSG( "P", P );
REGISTER_MSG( "PRIVMSG", PRIVMSG );

// Mode
REGISTER_MSG( "M", M );

// Quit
REGISTER_MSG( "Q", Q );

// BURST
REGISTER_MSG( "B", B );

// Join
REGISTER_MSG( "J", J ) ;

// Create
REGISTER_MSG( "C", C ) ;

// Leave
REGISTER_MSG( "L", L ) ;

// Part
REGISTER_MSG( "PART", PART ) ;

// Squit
REGISTER_MSG( "SQ", SQ ) ;
REGISTER_MSG( "SQUIT", SQ ) ;

// Kill
REGISTER_MSG( "D", D ) ;

// Invite
REGISTER_MSG( "I", I ) ;

// Wallops
REGISTER_MSG( "WA", WA ) ;

// Whois
REGISTER_MSG( "W", W ) ;

REGISTER_MSG( "R", R ) ;

// PASS
REGISTER_MSG( "PASS", PASS ) ;

// GLINE
REGISTER_MSG( "GL", GL ) ;

// TOPIC
REGISTER_MSG( "T", T ) ;

// KICK
REGISTER_MSG( "K", K ) ;

// No idea
REGISTER_MSG( "DS", DS ) ;

// Admin
REGISTER_MSG( "AD", AD ) ;

// Non-tokenized command handlers

// NOOP handlers.
// These messages aren't currently used.

// WHOIS
REGISTER_MSG( "WHOIS", NOOP ) ;

// PING Reply
REGISTER_MSG( "Z", NOOP ) ;

// MOTD
REGISTER_MSG( "MOTD", NOOP ) ;
REGISTER_MSG( "MO", NOOP ) ;

// STATS
REGISTER_MSG( "STATS", NOOP ) ;

// Version
REGISTER_MSG( "V", NOOP ) ;

// Trace
REGISTER_MSG( "TR", NOOP ) ;

// SETTIME
REGISTER_MSG( "SETTIME", NOOP ) ;

// End of channel ban list
REGISTER_MSG( "368", NOOP ) ;

// AWAY
REGISTER_MSG( "A", NOOP ) ;

// *shrug*
REGISTER_MSG( "441", NOOP ) ;

}

void xServer::initializeVariables()
{

// Initialize more variables
keepRunning = true ;
bursting = false ;
useBurstBuffer = false ;
_connected = false ;
StartTime = ::time( NULL ) ;

inputCharBuffer = 0 ;
caughtSignal = false ;
whichSig = 0 ;
burstStart = burstEnd = 0 ;
Uplink = NULL ;
theSock = NULL ;
Message = SRV_SUCCESS ;
outputWriteSize = inputReadSize = 0 ;
lastTimerID = 1 ;

// Initialize the numeric stuff.
::memset( charYY, 0, sizeof( charYY ) ) ;
::memset( charXXX, 0, sizeof( charXXX ) ) ;

Network = new (nothrow) xNetwork ;
assert( Network != 0 ) ;

Network->setServer( this ) ;

}

bool xServer::readConfigFile( const string& fileName )
{
// Read the configuration file and set our
// data members.
EConfig conf( fileName ) ;

// Parse out the required data fields
UplinkName = conf.Require( "uplink" )->second ;
ServerName = conf.Require( "name" )->second ;
ServerDescription = conf.Require( "description" )->second ;
Password = conf.Require( "password" )->second ;
Port = atoi( conf.Require( "port" )->second.c_str() ) ;
intYY = atoi( conf.Require( "numeric" )->second.c_str() ) ;
intXXX = atoi( conf.Require( "maxclients" )->second.c_str() ) ;

return true ;
}

bool xServer::loadModules( const string& fileName )
{
EConfig conf( fileName ) ;

/*
 * Load and attach any modules specified in the config.
 */ 
EConfig::const_iterator ptr = conf.Find( "module" ) ;
for( ; ptr != conf.end() && ptr->first == "module" ; ++ptr )
	{
	StringTokenizer modInfo(ptr->second) ;
	elog	<< "xServer> Found module: " << modInfo[0]
		<< " (Config: " << modInfo[1] << ")" << endl;

	moduleLoader< xClient* >* ml =
		new (nothrow) moduleLoader< xClient* >( modInfo[ 0 ] ) ;
	assert( ml != 0 ) ;

	moduleList.push_back(ml); // Add moduleLoader to list. 
	xClient* clientPtr = ml->loadObject(modInfo[1]);
	if( NULL == clientPtr )
		{
		elog	<< "xServer> Failed to instantiate module: "
			<< modInfo[ 1 ] << endl ;
		return false ;
		}
 
	AttachClient(clientPtr); 
	}

return true ;
}

void xServer::registerServerTimers()
{
RegisterTimer( ::time( 0 ) + 10, // start in 10 seconds
	new GlineUpdateTimer,
	static_cast< void* >( this ) ) ;
RegisterTimer( ::time( 0 ) + 60,
	new PINGTimer,
	static_cast< void* >( this ) ) ;
}
 
/**
 * Initiate a server shutdown.
 */
void xServer::Shutdown()
{
// TODO
}

/**
 * Connect to the given hostname/IP address on the given
 * port number.  This method returns 0 on success, -1 on error.
 * Upon successful completion of the connect, the server's
 * connection is ready to be written to.
 */
int xServer::Connect( const string& Address, int Port )
{

if( isConnected() )
	{
	OnDisConnect() ;
	Message = SRV_SUCCESS ;
	}

// The Socket may be non-null here when we are actually
// reading from a file.
if( 0 == theSock )
	{
	// Allocate the socket.
	theSock = new ClientSocket ;
	assert( theSock != 0 ) ;
	}

// P10 version information, bogus.
Version = 10 ;

// Perform the actual connect to the given IP/hostname.
if( theSock->connect( Address, Port ) < 0 )
	{
	return -1 ;
	}

// We got connected!

// Maintain connection state.
_connected = true ;

// Initialize the connection time variable to current time.
ConnectionTime = ::time( NULL ) ;

// Obtain the size of the TCP input window.
// The server will never attempt to read more bytes than this
// from the socket connection.
inputReadSize = theSock->recvBufSize() ;
if( static_cast< int >( inputReadSize ) < 0 )
	{
	elog	<< "xServer::Connect> Failed to get receive buffer size\n" ;
	return -1 ;
	}

// Obtain the size of the TCP output window.
// The server will never attempt to write more bytes than
// this to the socket connection.
outputWriteSize = theSock->sendBufSize() ;
if( static_cast< int >( outputWriteSize ) < 0 )
	{
	elog	<< "xServer::Connect> Failed to get output buffer size\n" ;
	return -1 ;
	}

inputCharBuffer = new (nothrow) char[ inputReadSize + 1 ] ;
assert( inputCharBuffer != 0 ) ;

// Notify the curious user of the TCP window sizes.
//elog << "inputReadSize: " << inputReadSize << endl ;
//elog << "outputWriteSize: " << outputWriteSize << endl ;

// Login to the uplink.
WriteDuringBurst( "PASS :%s\n", Password.c_str() ) ;

// Send our server information.
WriteDuringBurst( "SERVER %s %d %d %d J%02d %s :%s\n",
	        ServerName.c_str(),
		1,
		StartTime,
		ConnectionTime,
		Version,
		getCharYYXXX().c_str(),
		ServerDescription.c_str() ) ;

/*
// Wait for the writable state on the socket.
// Have to check by the socket itself here
// because ReadyForWrite() checks the output
// buffer first.
while( true )
	{
	int writable = theSock->writable() ;
	if( writable < 0 )
		{
		elog	<< "Socket failed after connect :(\n" ;

		OnDisConnect() ;
		return -1 ;
		}
	else if( 0 == writable )
		{
		::usleep( 10000 ) ;
		}
	else
		{
		break ;
		}
	}

// Flush the login/server information to the uplink.
flushBuffer() ;
*/

return 0 ;

}

/**
 * Handle a disconnect from our uplink.  This method is
 * responsible for deallocating variables mostly.
 */
void xServer::OnDisConnect()
{
if( theSock )
	{
	theSock->close() ;
	delete theSock ; theSock = 0 ;
	}

_connected = false ;
inputReadSize = outputWriteSize = 0 ;

// Clear socket buffers
inputBuffer.clear() ;
outputBuffer.clear() ;

Message = SRV_DISCONNECT ;

}

// This function parses and distributes incoming lines
// of data
void xServer::Process( char* s )
{
if( NULL == theSock || NULL == s || 0 == s[ 0 ] || ' ' == s[ 0 ] )
	{
	return ;
	}

// isNumeric indicates whether or not the first
// argument in the (s) array is a numeric
// n2k complicates things in this way...
bool		isNumeric = true ;
char		YXX[ 10 ] = { 0 } ;
char		*Command = NULL,
		*Sender = NULL ;

// Check for incoming numeric
if( s[ 0 ] != ':' )
	{

	char* yxxPtr = YXX ;
	while( *s && (' ' != *s) )
		{
		*yxxPtr++ = *s++ ;
		}

	// s now points to ' ' or is no longer valid
	// Increment past any white space, assigning
	// each as a NULL terminator as we pass.
	while( *s && (' ' == *s) )
		{
		*s++ = 0 ;
		}

	// s now points to a non white space character,
	// or is not valid
	if( !s || !(*s) )
		{
		return ;
		}

	// Extract the command
	Command = strtok( s, " " ) ;

	// According to my calculations, all
	// non-numeric commands that do not
	// begin with ':' have ':' as the
	// beginning of the second argument,
	// Im probably wrong though.
	//
	// Example:
	// ERROR :blah blah
	// PASS :blah
	//
	// Exception:
	// SERVER message, sent once, whose args do not
	// begin with ':'
	//
	if( (Command && ':' == *Command) || !strncmp( YXX, "SERV", 4 ) )
		{
		// It's not a command, the YXX
		// is not a numeric
		isNumeric = false ;
		}

	// Point Sender to the numeric of the sender,
	// whether it be server or client
	Sender = YXX ;

	} 

// :ripper.ufl.edu 442 EuWorld3 #nowhere :You're not on that channel
else
	// s[ 0 ] == ':'
	{
	// Sender will be the first argument, likely
	// the server from which this command
	// originates
	Sender = strtok( s, " " ) ;

	// Skip over the ':'
	++Sender ;

	// Extract the command
	Command = strtok( NULL, " " ) ;

	}

if( NULL == Sender )
	{
	elog	<< "xServer::Process> NULL == Sender... *shrug*\n" ;
	Command = strtok( s, " " ) ;
	}

// If commands like ERROR and PASS slipped
// into the first if structure above (as
// they should), then the Command will be bogus.
// Let's adjust the Command parameter.
if( !isNumeric )
	{
	// This is a bit of a hack
	// Sender right now points to YXX
	// Redirect it to point to the first
	// argument, so it will be the first
	// token put into the xParameters object
	// below.
	Sender = Command ;

	// Command becomes the original YXX
	Command = YXX ;
	}

// elog << "Sender: " << Sender << endl ;
// elog << "Command: " << Command << endl ;
// elog << "strlen( Command ): " << strlen( Command ) << endl ;

// Lookup the handler for this command
commandMapType::iterator pairPtr =
	commandMap->find( Command ) ;
if( pairPtr != commandMap->end() )
	{
	// Found a command handler for this
	// command.
	// Prepare the arguments for the command.

	char		*x = NULL ;
	xParameters 	Param ;

	// Was this command was sent from a server
	// or client?
	if( Sender )
		{
		Param << Sender ;
		}

	// Extract the argument list
	x = strtok( NULL, "\n" ) ;

	// Continue while there are characters
	// yet to parse
	while( x && *x )
		{

		// Set to NULL any space characters
		while( *x == ' ' )
			{
			*x++ = 0 ;
			}

		// Have we reached the end of the line?
		if( *x == 0 )
			{
			break ;
			}

		// x now points to a non-whitespace
		// character

		// Some arguments to commands are preceeded
		// by ':', skip it, but only if the preceeding
		// character was a white space (now NULL)
		// character.
		if( *x == ':' && *(x - 1) == 0 )
			{

			// Skip the ':'
			x++ ;

			// Put the rest of the string into
			// the xParameters instance
			// -> It is an argument string
			Param << x ;

			// We have reached the end of the
			// line, it has just been put into
			// the end of the xParameters instance
			// as a whole.
			break ;

			}

		// Add this argument to the xParameters
		// instance.
		Param << x ;

		// Skip the token just added to the xParameters.
		while( *x && *x != ' ' )
			{
			x++ ;
			}

		} // close while

	// Arguments are set.
	// Go ahead and call the handler method
	(this->*(pairPtr->second))( Param ) ;

	}
else
	{
	elog	<< "xServer::Process> Unable to find handler for: "
		<< Command << endl ;
	}
}

/**
 * Squit another server as a server.
 * 0 SQ server.name.com timestamp :reason
 */
bool xServer::SquitServer( const string& serverName, const string& reason )
{

// Is it our server?
if( !strcasecmp( serverName.c_str(), this->ServerName.c_str() ) )
	{
	// I don't see that happening
	elog	<< "xServer::SquitServer> Attempt to squit myself!\n" ;
	return false ;
	}

// All juped servers are also put into the Network tables...
// This call to findServerName() will find a juped server.
iServer* theServer = Network->removeServerName( serverName ) ;
if( NULL == theServer )
	{
	// The server doesn't exist.
	elog	<< "xServer::SquitServer> Unable to find server: "
		<< serverName << endl ;
	return false ;
	}

// Remove the server from the list of juped servers, if it is there.
for( jupedServerListType::iterator ptr = jupedServers.begin() ;
	ptr != jupedServers.end() ; ++ptr )
	{
	if( *ptr == theServer->getIntYY() )
		{
		// Found the server in the list of jupes
//		elog	<< "xServer::SquitServer> Found " << serverName
//			<< " in list of juped servers\n" ;
		jupedServers.erase( ptr ) ;
		break ;
		}
	}

// Don't really care if we found a server in the jupe list or not.

// Prepare the output buffer that will squit the server.
strstream s ;
s	<< charYY << " SQ " << serverName
	<< ' ' << time( 0 ) << " :" << reason << ends ;

// Notify the rest of the network of the SQUIT.
Write( s ) ;

// Deallocate stupid frozen strstream.
delete[] s.str() ;

// The server that is being squit has already been removed from
// both the network server table and the juped servers table.
// Deallocate the memory it occupies.
delete theServer ;

// TODO: Log event
// TODO: Post message

// Squit successful.
return true ;

}

/**
 * Attach a server.  This could be either a jupe, or some fictitious
 * server from which to host virtual clients.
 */
bool xServer::AttachServer( iServer* fakeServer, const string& description )
{
#ifndef NDEBUG
  assert( fakeServer != NULL ) ;
#endif

// Make sure a server of the same name is not already connected.
iServer* tmp = Network->findServerName( fakeServer->getName() ) ;

if( tmp != NULL )
	{
	// The server is already on the network.
	// Steal it's numeric :)
	fakeServer->uplinkIntYY = tmp->getIntYY() ;

	// Squit the old server and remove it from the internal tables.
	// This will also remove the server if it is already juped.
	SquitServer( fakeServer->getName(), "Being juped" ) ;

	// SquitServer() will also deallocate the server.
	// Make sure not to attempt to use the bogus tmp pointer.
	tmp = 0 ;

	}
else if( !jupedServers.empty() )
	{
	// Try to create a numeric by looking at the currently juped
	// servers.

	// Assume that the iServer's basic info such as name
	// and IP are set as they are desired by the caller.

	// Build a new numeric.
	// Find the last juped server.
	tmp = Network->findServer( jupedServers[ jupedServers.size() - 1 ] ) ;
	if( NULL == tmp )
		{
		elog	<< "xServer::AttachServer> Unable to find juped server: "
			<< jupedServers[ jupedServers.size() - 1 ] << endl ;
		return false ;
		}

	// Create a new numeric from the last juped server's numeric.
	fakeServer->intYY = tmp->getIntYY() + 1 ;
	}
else
	{
	// TODO: Try to make more sure that this numeric is available after
	// all bursting is complete -> reserve numeric?
	// Take a guess at a decent numeric.
	fakeServer->intYY = intYY + 100 ;
	}

// Setup the rest of the fake server's numeric stuff.
inttobase64( fakeServer->charYY, fakeServer->intYY, 2 ) ;

// Null terminate the server's new character numeric.
fakeServer->charYY[ 2 ] = 0 ;

//elog << "AttachServer> Built server: " << *fakeServer << endl ;

// Setup YYXXX numeric for new server...bogus
string charYYXXX( fakeServer->getCharYY() ) ;

// Guess at the xxx part of the numeric, the max number of clients
// for this server.
charYYXXX += "]]]" ;

// Burst the new server's info.
Write( "%s S %s %d %d %d P%02d %s 0 :Juped Server\n",
		getCharYY(),
		fakeServer->getName().c_str(),
		2,
		0, 
		fakeServer->getConnectTime(),
		10, // version
		charYYXXX.c_str(),
		description.c_str() ) ;

// Write burst acknowledgements.
Write( "%s EB", fakeServer->getCharYY() ) ;
Write( "%s EA", fakeServer->getCharYY() ) ;

// Add this fake server to the internal list of juped servers.
jupedServers.push_back( fakeServer->getIntYY() ) ;

// Add to network tables...It doesn't know whether it is fake
// or not, and why should it care?
Network->addServer( fakeServer ) ;

//elog	<< "xServer::AttachServer> Added server: "
//	<< fakeServer->getName() << endl ;

// Success.
return( 0 ) ;

}

/**
 * This method will register the given xClient to receive
 * all events of the given type.
 * Available events are listed in include/events.h
 */
bool xServer::RegisterEvent( const eventType& theEvent, xClient* theClient )
{
#ifndef NDEBUG
  assert( theClient != NULL ) ;
#endif

// Make sure that the given event is valid
// (in the interval of possible events).
if( !validEvent( theEvent ) )
	{
	return false ;
	}

// Make sure not to add a client more than once
UnRegisterEvent( theEvent, theClient ) ;

// Add this client as listener for this event
eventList[ theEvent ].push_back( theClient ) ;

// Registration succeeded
return true ;

}

/**
 * This method will register the given xClient for any
 * channel event that occurs in channel chanName, case
 * insensitive.
 */
bool xServer::RegisterChannelEvent( const string& chanName,
	xClient* theClient )
{
#ifndef NDEBUG
  assert( theClient != NULL ) ;
#endif

// Prevent duplicates of the same channel/client pair
UnRegisterChannelEvent( chanName, theClient ) ;

// Obtain a pointer to the list of xClient's registered for events
// on the given channel.
channelEventMapType::iterator chanPtr = channelEventMap.find( chanName ) ;
if( chanPtr == channelEventMap.end() )
	{
	// Channel event list doesn't exist yet
	channelEventMap.insert( channelEventMapType::value_type( chanName,
		new list< xClient* > ) ) ;
	chanPtr = channelEventMap.find( chanName ) ;
	}

// Add the xClient as a listener for events in this channel.
chanPtr->second->push_back( theClient ) ;

// Addition successful
return true ;

}

/**
 * This method will attempt to unregister the given client
 * from receiving events of type theEvent.
 * It will fail (return false) when theEvent is not valid, or
 * the xClient is not found as being registered for
 * event theEvent.
 */
bool xServer::UnRegisterEvent( const eventType& theEvent, xClient* theClient )
{
#ifndef NDEBUG
  assert( theClient != NULL ) ;
#endif

// Make sure this is a valid event.
if( !validEvent( theEvent ) )
	{
	return false ;
	}

// Iterate through the list of registered listeners for
// this event and attempt to find the xClient wishing to be removed.
//
list< xClient* >::iterator ptr =	eventList[ theEvent ].begin(),
					end = eventList[ theEvent ].end() ;

// Continue until we find the xClient.
// Since each xClient may only be registered for any given
// we may return as soon as we find the client.
while( ptr != end )
	{
	// Is this the one?
	if( (*ptr) == theClient )
		{
		// Yup, remove it and return true
		eventList[ theEvent ].erase( ptr ) ;
		return true ;
		}
	++ptr ;
	}

// Unable to find the client in the list of registered
// listeners for this event. *shrug*
return false ;

}

/**
 * This method will stop the given xClient from receiving any
 * events in the channel chanName, case insensitive.
 */
bool xServer::UnRegisterChannelEvent( const string& chanName,
	xClient* theClient )
{
#ifndef NDEBUG
  assert( theClient != NULL ) ;
#endif

channelEventMapType::iterator chanPtr = channelEventMap.find( chanName ) ;
if( chanPtr == channelEventMap.end() )
	{
	// Channel has no xClient's registered for channel events
	// No big deal.
	return true ;
	}

list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	if( *ptr == theClient )
		{
		listPtr->erase( ptr ) ;

		// Are there any listeners remaining for this channel?
		if( listPtr->empty() )
			{
			// Nope, remove the listener list from the
			// channelEventMap and deallocat the list.
			channelEventMap.erase( chanName ) ;
			delete listPtr ;
			}

		return true ;
		}
	}

// Unable to find the key/xClient pair.
return false ;
}

/**
 * This method will distribute to each xClient listening
 * for the given event (theEvent) an event with the proper
 * arguments.
 * Events are not guaranteed to be distributed in any
 * particular order.
 */
void xServer::PostEvent( const eventType& theEvent,
	void* Data1, void* Data2,
	void* Data3, void* Data4 )
{

// Make sure the event is valid.
if( !validEvent( theEvent ) )
	{
	elog	<< "xServer::PostEvent> Invalid event number: "
		<< theEvent << endl ;
	return ;
	}

// Iterate through the list of listeners for this event.
list< xClient* >::iterator ptr = eventList[ theEvent ].begin(),
	end = eventList[ theEvent ].end() ;

// Continue while there are more listeners for this event.
for( ; ptr != end ; ++ptr )
	{
	// Notify this client of the event
	(*ptr)->OnEvent( theEvent, Data1, Data2, Data3, Data4 ) ;
	}
}

/**
 * This method will distribute to any xClient registered to
 *  receive events for the given channel (chanName), case
 *  insensitive, an event with the proper arguments.
 * Events are not guaranteed to be distributed in any
 *  particular order.
 */
void xServer::PostChannelEvent( const channelEventType& theEvent,
	Channel* theChan,
	void* Data1, void* Data2,
	void* Data3, void* Data4 )
{

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelEvent( theEvent, theChan,
			Data1, Data2, Data3, Data4 ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelEvent( theEvent, theChan,
		Data1, Data2, Data3, Data4 ) ;
	}
}

// Attempt a read from the network connection.
// Returns the number of bytes read or -1 on error.
void xServer::DoRead()
{

if( !_connected || (NULL == theSock) )
	{
	elog	<< "xServer::DoRead> Not connected, or invalid "
		<< "socket" << endl ;

	// Make sure the caller knows that there is no
	// valid connection
	_connected = false ;
	return ;
	}

memset( inputCharBuffer, 0, inputReadSize + 1 ) ;

int bytesRead = theSock->recv(
	reinterpret_cast< unsigned char* >( inputCharBuffer ),
	inputReadSize ) ;

// Was the read successful?
if( bytesRead <= 0 )
	{
	// Read error, the connection is no longer valid
	_connected = false ;

	elog	<< "xServer::DoRead> Got a read error: "
		<< strerror( errno ) << endl ;
	}
else
	{
	inputBuffer += inputCharBuffer ;
	}

}

void xServer::DoWrite()
{

if( !_connected || (NULL == theSock) )
	{
	elog	<< "xServer::DoWrite> Attempt to write, but no "
		<< "connection!" << endl ;

	// Make sure the caller knows that there is no
	// valid connection
	_connected = false ;
	return ;
	}

if( outputBuffer.empty() )
	{
	elog	<< "xServer::DoWrite> Output buffer empty"
		<< endl ;
	return ;
	}

int bytesWritten = theSock->send( outputBuffer.toString(),
	min( outputWriteSize >> 1, outputBuffer.size() ) ) ;

if( bytesWritten <= 0 )
	{
	// Write error
	_connected = false ;
	}
else
	{
	outputBuffer.Delete( bytesWritten ) ;
	}
}

/**
 * Force a write of the output buffer to
 * the network.
 * Returns false if there is no valid connection
 * or the write fails.
 * Returns true otherwise.
 */
bool xServer::flushBuffer()
{

// Make sure there is a valid connection.
if( !_connected || !theSock )
	{
	// No connection, return false.
	return false ;
	}

// Does the output buffer have any data to
// be written?
if( 0 == outputBuffer.size() )
	{
	// Nope, return true.
	return true ;
	}

// Write the data to the network.
// TODO: Only write ouputWriteSize bytes.
int bytesWritten = theSock->send( outputBuffer.toString(),
	min( outputWriteSize >> 1, outputBuffer.size() ) ) ;

// Was the write successful?
if( bytesWritten < 0 )
	{
	// Nope, write error.  The connection
	// is no longer valid.
	OnDisConnect() ;

	// Return failure
	return false ;
	}

// The write was successful.
// Remove from the output buffer the data
// was written.
outputBuffer.Delete( bytesWritten ) ;

// Return success.
return true ;

}

/**
 * Return true is the socket is ready to be read.
 * Return false if the socket has no pending data,
 * or if the connection is invalid.
 */
bool xServer::ReadyForRead()
{
if( !_connected || !theSock )
	{
	return false ;
	}
int readable = theSock->readable() ;
if( readable < 0 )
	{
	// Error on socket
	OnDisConnect() ;
	return false ;
	}
return (readable > 0) ;
}

/**
 * Return true if the output buffer is not empty and
 * the conncetion is valid.
 * Return false otherwise.
 */
bool xServer::ReadyForWrite() const
{
return ( _connected && theSock &&
	(outputBuffer.size() > 0) && (theSock->writable() > 0) ) ;
}

/**
 * Read a \n terminated string from the socket
 * input buffer.
 * Make sure buf is sized large enough for any string
 * that may be sent across the network.
 */
bool xServer::GetString( char* buf )
{

// Is there a valid connection and data to
// be read?
if( !_connected || inputBuffer.empty() )
	{
	// Nope, return false
	return false ;
	}

// Read a line from the input buffer.
string tmp ;
if( !inputBuffer.ReadLine( tmp ) )
	{
	// No data to be read.
	// This is not fatal.
	return false ;
	}

// Remove any trailing \r or \n
for( char c = tmp[ tmp.size() - 1 ] ; c == '\r' || c == '\n' ;
	c = tmp[ tmp.size() - 1 ] )
	{
	tmp.erase( tmp.size() - 1 ) ;
	}

// Copy the line into the return buffer.
strcpy( buf, tmp.c_str() ) ;

// Return success.
return true ;

}

/**
 * Attach an xClient to the server.
 */
bool xServer::AttachClient( xClient* Client )
{

// Make sure the pointer is valid.
#ifndef NDEBUG
  assert( NULL != Client ) ;
#endif

// addClient() will allocate a new YYXXX and
// update Client.
if( !Network->addClient( Client ) )
	{
	elog	<< "xServer::AttachClient> Network->addClient failed\n" ;
	return false ;
	}

// Let the client know it has been added to
// the server and its tables.
Client->ImplementServer( this ) ;

iClient* theIClient = new (nothrow) iClient(
	getIntYY(),
	Client->getCharYYXXX(),
	Client->getNickName(),
	Client->getUserName(),
	"AAAAAA",
	Client->getHostName(),
	Client->getModes(),
	Client->getDescription(),
	::time( 0 ) ) ;
assert( theIClient != 0 ) ;

Client->setInstance( theIClient ) ;

if( !Network->addClient( theIClient ) )
	{
	elog	<< "xServer::AttachClient> Unable to add theIClient "
		<< "to the Network table"
		<< endl ;
	return false ;
	}

return true ;
}

/**
 * Attach an iClient to a juped server.
 *
 * AQ N ripper_ 1 952038834 ~dan 127.0.0.1 +owg B]AAAB AQAAA :Dan Karrels
 */
bool xServer::AttachClient( iClient* fakeClient )
{
#ifndef NDEBUG
  assert( fakeClient != NULL ) ;
#endif

// Need to send info to the network about the new client.
iServer* fakeServer = Network->findServer( fakeClient->getIntYY() ) ;
if( NULL == fakeServer )
	{
	elog	<< "xServer::AttachClient> Unable to find fake server: "
		<< fakeClient->getIntYY() << endl ;
	return -1 ;
	}

Write( "%s N %s 2 %d %s %s +d %s %s :%s\n",
	fakeServer->getCharYY(),
	fakeClient->getNickName().c_str(),
	fakeClient->getConnectTime(),
	fakeClient->getUserName().c_str(),
	fakeClient->getInsecureHost().c_str(),
	xIP( fakeClient->getIP() ).GetBase64IP(),
	fakeClient->getCharYYXXX().c_str(),
	"Clone" ) ;

return Network->addClient( fakeClient ) ;
}

/**
 * Detach an xClient from the xServer.
 */
bool xServer::DetachClient( xClient* Client )
{
#ifndef NDEBUG
  assert( NULL != Client ) ;
#endif

// Notify the client that it is being detached.
Client->Exit( "Client has been detached by server" ) ;

// TODO: Unregister client from event tables

// Remove the client from the internal tables and
// deallocate its space.
delete Network->findLocalClient( Client->getIntYY(), Client->getIntXXX() ) ;

return true ;
}

/**
 * Detach an xClient by nickname.
 */
bool xServer::DetachClient( const string& Nick )
{
return DetachClient( Network->findLocalNick( Nick ) ) ;
}

/**
 * This method will append a string to the output
 * buffer.
 * Returns false if there is no valid connection,
 * true otherwise.
 */
size_t xServer::Write( const string& buf )
{

// Is there a valid connection?
if( !_connected )
	{
	return 0 ;
	}

if( verbose )
	{
	// Output the debugging information
	// to the console.
	clog << "[OUT]: " << buf  ;

	// Should we output a trailing newline
	// character?
	if( buf[ buf.size() - 1 ] != '\n' )
		{
		cout << endl ;
		}
	}

// Newline terminate the string if it's
// not already done and append it to
// the output buffer.
//
if( buf[ buf.size() - 1 ] != '\n' )
	{
	if( useBurstBuffer )
		{
		burstOutputBuffer += buf + '\n' ;
		}
	else
		{
		outputBuffer += buf + '\n' ;
		}
	}
else
	{
	if( useBurstBuffer || bursting )
		{
		burstOutputBuffer += buf + '\n' ;
		}
	else
		{
		outputBuffer += buf ;
		}
	}

// Return success.
return buf.size() ;

}

size_t xServer::WriteDuringBurst( const string& buf )
{

// Is there a valid connection?
if( !_connected )
	{
	return 0 ;
	}

if( verbose )
	{
	// Output the debugging information
	// to the console.
	clog << "[OUT]: " << buf  ;

	// Should we output a trailing newline
	// character?
	if( buf[ buf.size() - 1 ] != '\n' )
		{
		cout << endl ;
		}
	}

// Newline terminate the string if it's
// not already done and append it to
// the output buffer.
//
if( buf[ buf.size() - 1 ] != '\n' )
	{
	outputBuffer += buf + '\n' ;
	}
else
	{
	outputBuffer += buf ;
	}

// Return success.
return buf.size() ;

}

/**
 * Write the contents of a std::strstream to the uplink connection.
 */
size_t xServer::Write( strstream& s )
{
return Write( string( s.str() ) ) ;
}

size_t xServer::WriteDuringBurst( strstream& s )
{
return WriteDuringBurst( string( s.str() ) ) ;
}

/**
 * This method appends the variable sized argument
 * list buffer to the output buffer.
 * Returns false if no valid connection available,
 * true otherwise.
 * I despise this function. --dan
 */
size_t xServer::Write( const char* format, ... )
{

// Is there a valid connection?
if( !_connected )
	{
	// Nope, return false.
	return false ;
	}

// Go through the motions of putting the
// string into a buffer.
char buffer[ 4096 ] = { 0 } ;
va_list _list ;

va_start( _list, format ) ;
vsprintf( buffer, format, _list ) ;
va_end( _list ) ;

#ifdef EDEBUG
	// Output the string to the console.
	cout << "[OUT]: " << buffer  ;

	// Do we need to newline terminate it?
	if( buffer[ strlen( buffer ) - 1 ] != '\n' )
		{
		cout << endl ;
		}
#endif

if( useBurstBuffer )
	{
	burstOutputBuffer += buffer ;
	}
else
	{
	// Append the line to the output buffer.
	outputBuffer += buffer ;
	}

// Return number of bytes written
return strlen( buffer ) ;

}

size_t xServer::WriteDuringBurst( const char* format, ... )
{

// Is there a valid connection?
if( !_connected )
	{
	// Nope, return false.
	return false ;
	}

// Go through the motions of putting the
// string into a buffer.
char buffer[ 4096 ] = { 0 } ;
va_list _list ;

va_start( _list, format ) ;
vsprintf( buffer, format, _list ) ;
va_end( _list ) ;

#ifdef EDEBUG
	// Output the string to the console.
	cout << "[OUT]: " << buffer  ;

	// Do we need to newline terminate it?
	if( buffer[ strlen( buffer ) - 1 ] != '\n' )
		{
		cout << endl ;
		}
#endif

// Append the line to the output buffer.
outputBuffer += buffer ;

// Return number of bytes written.
return strlen( buffer ) ;

}

bool xServer::removeGline( const string& userHost )
{
glineListType::iterator ptr = glineList.begin() ;
for( ; ptr != glineList.end() ; ++ptr )
	{
	if( *(*ptr) == userHost )
		{
		break ;
		}
	++ptr ;
	}

if( ptr == glineList.end() )
	{
	return false ;
	}

// Found it
strstream s ;
s	<< charYY
	<< " GL * -"
	<< userHost
	<< ends ;

Write( s ) ;
delete[] s.str() ;

glineList.erase( ptr ) ;
PostEvent( EVT_REMGLINE,
	static_cast< void* >( *ptr ) ) ;

delete *ptr ;
return true ;
}

// C GL * +~*@209.9.117.131 180 :Banned (~*@209.9.117.131) until 957235403 (On Mon May  1
// 22:40:23 2000 GMT from SE5 for 180 seconds: remgline test.. 	[0])
bool xServer::setGline(
	const string& setBy,
	const string& userHost,
	const string& reason,
	const time_t& duration )
{

// TODO: Remove any old matches, or just update the expiration time

Gline* newGline =
	new (nothrow) Gline( setBy, userHost, reason, duration ) ;
assert( newGline != 0 ) ;

// Notify the rest of the network
strstream s ;
s	<< charYY << " GL * +"
	<< userHost << ' '
	<< (::time( 0 ) + duration) << " :"
	<< reason << ends ;
Write( s ) ;
delete[] s.str() ;

glineList.push_back( newGline ) ;
PostEvent( EVT_GLINE,
	static_cast< void* >( newGline ) ) ;

return true ;
}

// TODO
vector< const Gline* > xServer::matchGline( const string& userHost ) const
{
return vector< const Gline* >() ;
}

const Gline* xServer::findGline( const string& userHost ) const
{
for( glineListType::const_iterator ptr = glineList.begin() ;
	ptr != glineList.end() ; ++ptr )
	{
	// TODO: Case insensitive search
	if( *(*ptr) == userHost )
		{
		// Found it
		return *ptr ;
		}
	}
return 0 ;
}

/**
 * Write an xClient channel part to the network, and update
 * network tables.
 */
void xServer::PartChannel( xClient* theClient, const string& chanName, const string& reason )
{
#ifndef NDEBUG
  assert( theClient != NULL ) ;
#endif

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::PartChannel> Unable to find channel: "
		<< chanName << endl ;
	return ;
	}

PartChannel( theClient, theChan, reason ) ;
}

/**
 * Write an xClient channel part to the network, and update
 * network tables.
 */
void xServer::PartChannel( xClient* theClient, Channel* theChan, const string& reason )
{
#ifndef NDEBUG
  assert( theClient != NULL && theChan != NULL ) ;
#endif

strstream s ;
s	<< theClient->getCharYYXXX() << " L "
	<< theChan->getName() << " :" << reason << ends ;

Write( s ) ;
delete[] s.str() ;

OnPartChannel( theClient, theChan ) ;
OnPartChannel( theClient->getInstance(), theChan ) ;
}

/**
 * Handle an iClient channel part.  The reason for having this
 * method in addition to MSG_L() is that one of the attached
 * xClient's may KICK a client from a channel.  In that case
 * no MSG_L() or MSG_K() is read from the network.  This method
 * allows an xClient to update the internal tables without
 * needing to know exactly what needs to be done (encapsulation).
 */
void xServer::OnPartChannel( iClient* theClient, const string& chanName )
{
#ifndef NDEBUG
  assert( theClient != NULL ) ;
#endif

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::OnPartChannel> Unable to find channel: "
		<< chanName << endl ;
	return ;
	}

OnPartChannel( theClient, theChan ) ;
}

/**
 * Handle an iClient channel part.  The reason for having this
 * method in addition to MSG_L() is that one of the attached
 * xClient's may KICK a client from a channel.  In that case
 * no MSG_L() or MSG_K() is read from the network.  This method
 * allows an xClient to update the internal tables without
 * needing to know exactly what needs to be done (encapsulation).
 */
void xServer::OnPartChannel( iClient* theClient, Channel* theChan )
{
#ifndef NDEBUG
  assert( theClient != 0 ) ;
  assert( theChan != 0 ) ;
#endif

theClient->removeChannel( theChan ) ;
delete theChan->removeUser( theClient ) ;

PostChannelEvent( EVT_PART, theChan,
	static_cast< void* >( theClient ) ) ;

if( theChan->empty() )
	{
	// Empty channel
	delete Network->removeChannel( theChan ) ;
	}

}

void xServer::OnPartChannel( xClient* theClient, const string& chanName )
{
#ifndef NDEBUG
  assert( theClient != NULL ) ;
#endif

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::OnPartChannel> Unable to find channel: "
		<< chanName << endl ;
	return ;
	}
OnPartChannel( theClient, theChan ) ;

}

void xServer::OnPartChannel( xClient* theClient, Channel* theChan )
{
#ifndef NDEBUG
  assert( theClient != NULL && theChan != NULL ) ;
#endif

}

void xServer::JoinChannel( xClient* theClient, const string& chanName,
	const string& chanModes,
	const time_t& joinTime, bool getOps )
{

// Determine the timestamp to use for the join
time_t postJoinTime = joinTime ;
if( 0 == postJoinTime )
	{
	postJoinTime = ::time( 0 ) ;
	}

Channel* theChan = Network->findChannel( chanName ) ;
if( theChan && (0 == joinTime) )
	{
	// If the channel exists, and 0 is passed as our join
	// time, then just use the channel's creation time
	postJoinTime = theChan->getCreationTime() ;
	}

if( (NULL == theChan) && bursting )
	{
	// Need to burst the channel
	strstream s ;
	s	<< getCharYY() << " B " << chanName << ' '
		<< postJoinTime << ' '
		<< chanModes << ' '
		<< theClient->getCharYYXXX() ;

	if( getOps )
		{
		s	<< ":o" ;
		}

		s	<< ends ;

	Write( s ) ;
	delete[] s.str() ;

	// Instantiate the new channel
	theChan = new (nothrow) Channel( chanName, time( 0 ) ) ;
	assert( theChan != 0 ) ;

	// Add it to the network channel table
	Network->addChannel( theChan ) ; 

	}
else if( NULL == theChan )
	{
	// Channel doesn't exist yet, and we're NOT bursting
	// 0AT C #lksjhdlksjdlkjs 957214787

//	elog	<< "xServer::BurstChannel> Creating new channel: "
//		<< chanName << endl ;

		// Create the channel
		// The client automatically gets op in this case
	{
	strstream s ;
	s	<< theClient->getCharYYXXX() << " C "
		<< chanName << ' ' << postJoinTime ;
	Write( s ) ;
	delete[] s.str() ;
	}

	if( !chanModes.empty() )
		{
		strstream s ;
		s	<< theClient->getCharYYXXX()
			<< " M " << chanName << ' '
			<< chanModes << ends ;
		Write( s ) ;
		delete[] s.str() ;
		}

	// Instantiate the new channel
	theChan = new (nothrow) Channel( chanName, time( 0 ) ) ;
	assert( theChan != 0 ) ;

	// Add it to the network channel table
	Network->addChannel( theChan ) ;

	}
else if( bursting )
	{
	// Channel exists, still bursting
	// 0 B #coder-com 000031337 +tn 0AT,EAA:o,KAB,0AA

	// Is the timestamp we are bursting older than the current
	// timestamp?
	if( postJoinTime < theChan->getCreationTime() )
		{
		// We are bursting an older timestamp
		// Remove all modes
		removeAllChanModes( theChan ) ;
		}

	// TODO: If the timestamp we are bursting is less than the
	// existing one, we need to set the our Network channel state to
	// match that supplied in this line. (Because we are authoritive
	// in this channel, any existing modes will be removed by ircu).
	strstream s ;
	s	<< getCharYY() << " B " << chanName << ' '
		<< postJoinTime << ' '
		<< chanModes << ' '
		<< theClient->getCharYYXXX() ;

	if( getOps )
		{
		s	<< ":o" ;
		}

		s	<< ends ;

	Write( s ) ;
	delete[] s.str() ;
	}
else
	{
	// After bursting, and the channel exists
		{
		strstream s2 ;
		s2	<< theClient->getCharYYXXX() << " J "
			<< chanName << ends ;
		Write( s2 ) ;
		delete[] s2.str() ;
		}

	if( getOps )
		{
		// Op the bot
		strstream s ;
		s	<< charYY << " M " << chanName
			<< " +o " << theClient->getCharYYXXX()
			<< ends ;
		Write( s ) ;
		delete[] s.str() ;
		}

	if( !chanModes.empty() )
		{
		// Set the channel modes
		strstream s ;
		s	<< theClient->getCharYYXXX() << " M "
			<< chanName << ' '
			<< chanModes << ends ;

		Write( s ) ;
		delete[] s.str() ;
		}
	}

if( postJoinTime < theChan->getCreationTime() )
	{
	theChan->setCreationTime( joinTime ) ;
	}

if( !chanModes.empty() )
	{
	StringTokenizer st( chanModes ) ;
	StringTokenizer::size_type argPos = 1 ;

	for( string::const_iterator ptr = chanModes.begin() ;
		ptr != chanModes.end() ; ++ptr )
		{
		switch(  *ptr )
			{
			case 't':
				theChan->onModeT( true ) ;
				break ;
			case 'n':
				theChan->onModeN( true ) ;
				break ;
			case 's':
				theChan->onModeS( true ) ;
				break ;
			case 'p':
				theChan->onModeP( true ) ;
				break ;
			case 'm':
				theChan->onModeM( true ) ;
				break ;
			case 'i':
				theChan->onModeI( true ) ;
				break ;
			case 'k':
				{
				if( argPos >= st.size() )
					{
					elog	<< "xServer::JoinChannel> Invalid"
						<< " number of arguments to "
						<< "chanModes"
						<< endl ;
					break ;
					}
				theChan->onModeK( true, st[ argPos++ ] ) ;
				break ;
				}
			case 'l':
				{
				if( argPos >= st.size() )
					{
					elog	<< "xServer::JoinChannel> Invalid"
						<< " number of arguments to "
						<< "chanModes"
						<< endl ;
					break ;
					}
				theChan->onModeL( true,
					atoi( st[ argPos++ ].c_str() ) ) ;
				break ;
				}
			} // switch()
		} // for()
	} // if( !chanModes.empty() )

// An xClient has joined a channel, update its iClient instance
iClient* theIClient = theClient->getInstance() ;

theIClient->addChannel( theChan ) ;
theChan->addUser( theIClient ) ;

}

// K N Isomer 2 957217279 ~perry p136-tnt1.ham.ihug.co.nz DLbaCI KAC :*Unknown*
void xServer::BurstClient( xClient* theClient, bool localClient )
{
strstream s ;
s	<< charYY << " N "
	<< theClient->getNickName() << ' '
	<< (localClient ? '1' : '2') << " 31337 "
	<< theClient->getUserName() << ' '
	<< theClient->getHostName() << ' '
	<< theClient->getModes() << ' '
	<< "AAAAAA" << ' '
	<< theClient->getCharYYXXX() << " :"
	<< theClient->getDescription() << ends ;
Write( s ) ;
delete[] s.str() ;
theClient->Connect( 31337 ) ;
}

void xServer::BurstClients()
{
xNetwork::localClientIterator ptr = Network->localClient_begin() ;
while( ptr != Network->localClient_end() )
	{
	BurstClient( *ptr ) ;
	++ptr ;
	}
}

void xServer::BurstChannels()
{
xNetwork::localClientIterator ptr = Network->localClient_begin() ;
while( ptr != Network->localClient_end() )
	{
	(*ptr)->BurstChannels() ;
	++ptr ;
	}
}

// Burst all client info, but NOT channel info.
// TODO: Burst juped servers.
void xServer::Burst()
{

xNetwork::localClientIterator ptr = Network->localClient_begin(),
	end = Network->localClient_end() ;

while( ptr != end )
	{
	// This should never happen at burst time,
	// but just to be safe...
	if( NULL == *ptr )
		{
		continue ;
		}
	(*ptr)->Connect( 0x31337 ) ;

	// No need to add to tables, it is
	// already there
	++ptr ;
	}

// TODO: Need to burst fake servers and clients

}

void xServer::dumpStats()
{
clog	<< "Number of channels: " << Network->channelList_size() << endl ;
clog	<< "Number of servers: " << Network->serverList_size() << endl ;
clog	<< "Number of clients: " << Network->clientList_size() << endl ;
clog	<< "Burst duration: " << (burstEnd - burstStart) << " seconds\n" ;
}

xServer::timerID xServer::RegisterTimer( const time_t& absTime,
	TimerHandler* theHandler,
	void* data )
{
#ifndef NDEBUG
  assert( theHandler != 0 ) ;
#endif

// Don't register a timer that has already expired.
if( absTime <= ::time( 0 ) )
	{
	return 0 ;
	}

// Retrieve a unique timerID
timerID ID = getUniqueTimerID() ;

// Allocate a timerInfo structure to represent this timer
timerInfo* ti =
	new (nothrow) timerInfo( ID, absTime, theHandler, data ) ;
assert( ti != 0 ) ;

// Add this timerInfo structure to the timerQueue
timerQueue.push( timerQueueType::value_type( absTime, ti ) ) ;

// Add the unique timerID to the timerID map
uniqueTimerMap.insert( uniqueTimerMapType::value_type( ID, true ) ) ;

// Return the valid timerID of this timer
return ID ;
}

bool xServer::UnRegisterTimer( const xServer::timerID& ID,
	void*& data )
{

// Make sure there are timers in the queue
if( timerQueue.empty() )
	{
	// The timerQueue is empty -- the timerID specified
	// is invalid at best.
	return true ;
	}

// Create a local stack to store elements popped from the timerQueue
// in searching for the timer in question (could also have used
// another priority queue)
stack< timerQueueType::value_type > localStack ;

// Continue while the timerQueue is not empty, and we have not
// found the timerID in question.
while( !timerQueue.empty() && (timerQueue.top().second->ID != ID) )
	{
	// Add this timer to the local stack
	localStack.push( timerQueue.top() ) ;

	// Remove this timer from the timerQueue
	timerQueue.pop() ;
	}

// This variable will represent the case that we found
// the timer in question
bool foundTimer = false ;

// timerQueue is now empty, or its top element has the timer
// we are interested in.
if( !timerQueue.empty() )
	{
	// Find was successful
	foundTimer = true ;

	// Assign the timer argument back to "data"
	if( data != 0 )
		{
		data = timerQueue.top().second->data ;
		}

	// Remove this timerID from the uniqueTimerMap
	uniqueTimerMap.erase( timerQueue.top().second->ID ) ;

	// Deallocate the timerInfo structure for this timer
	delete timerQueue.top().second ;

	// Remove the timerInfo from the timerQueue
	timerQueue.pop() ;
	}

// Put the rest of the timers back onto the timerQueue.
while( !localStack.empty() )
	{
	timerQueue.push( localStack.top() ) ;
	localStack.pop() ;
	}

return foundTimer ;
}

unsigned int xServer::CheckTimers()
{

// Make sure the timerQueue is not empty, and that
// we are not bursting
if( timerQueue.empty() )
	{
	return 0 ;
	}

// Create a variable to count the number of timers executed
unsigned int numTimers = 0 ;

// What time is this method being invoked?
time_t now = ::time( 0 ) ;

while( !timerQueue.empty() && (timerQueue.top().second->absTime <= now) )
	{
	// Grab a timerInfo structure
	timerInfo* info = timerQueue.top().second ;

	// Remove the structure from the timerQueue
	timerQueue.pop() ;

	// Call the timer handler method for the client
	info->theHandler->OnTimer( info->ID, info->data ) ;

	// Remove the timerID from the uniqueTimerMap
	uniqueTimerMap.erase( info->ID ) ;

	// Deallocate the timerInfo structure for this timer
	delete info ;

	// Increment the counter for number of timers executed
	++numTimers ;
	}

// Return the number of timers executed
return numTimers ;
}

xServer::timerID xServer::getUniqueTimerID()
{
timerID retMe = lastTimerID++ ;
while( uniqueTimerMap.find( retMe ) != uniqueTimerMap.end() )
	{
	retMe = lastTimerID++ ;
	}
return retMe ;
}

bool xServer::PostSignal( int whichSig )
{

// First, notify the server signal handler
bool handledSignal = OnSignal( whichSig ) ;

Network->foreach_xClient( handleSignal( whichSig ) ) ;

/*
// Pass this signal on to each xClient.
xNetwork::localClientIterator ptr = Network->localClient_begin() ;
for( ; ptr != Network->localClient_end() ; ++ptr )
	{
	if( NULL == *ptr )
		{
		continue ;
		}
	(*ptr)->OnSignal( whichSig ) ;
	}
*/

return handledSignal ;
}

bool xServer::OnSignal( int whichSig )
{
bool retMe = false ;
switch( whichSig )
	{
	case SIGUSR1:
		dumpStats() ;
		retMe = true ;
		break ;
	case SIGHUP:
		retMe = true ;
		break ;
	default:
		break ;
	}
return retMe ;
}

void xServer::run()
{
mainLoop() ;
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeT( Channel* theChan, bool polarity,
	ChannelUser* sourceUser )
{
theChan->onModeT( polarity ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeT( theChan, polarity, sourceUser ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeT( theChan, polarity, sourceUser ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeN( Channel* theChan, bool polarity,
	ChannelUser* sourceUser )
{
theChan->onModeN( polarity ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeN( theChan, polarity, sourceUser ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeN( theChan, polarity, sourceUser ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeS( Channel* theChan, bool polarity,
	ChannelUser* sourceUser )
{
theChan->onModeS( polarity ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeS( theChan, polarity, sourceUser ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeS( theChan, polarity, sourceUser ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeP( Channel* theChan, bool polarity,
	ChannelUser* sourceUser )
{
theChan->onModeP( polarity ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeP( theChan, polarity, sourceUser ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeP( theChan, polarity, sourceUser ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeM( Channel* theChan, bool polarity,
	ChannelUser* sourceUser )
{
theChan->onModeM( polarity ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeM( theChan, polarity, sourceUser ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeM( theChan, polarity, sourceUser ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeI( Channel* theChan, bool polarity,
	ChannelUser* sourceUser )
{
theChan->onModeI( polarity ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeI( theChan, polarity, sourceUser ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeI( theChan, polarity, sourceUser ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeL( Channel* theChan, bool polarity,
	ChannelUser* sourceUser, unsigned int limit )
{
theChan->onModeL( polarity, limit ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeL( theChan, polarity,
			sourceUser, limit ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeL( theChan, polarity, sourceUser, limit ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeK( Channel* theChan, bool polarity,
	ChannelUser* sourceUser, const string& key )
{
theChan->onModeK( polarity, key ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeK( theChan, polarity,
			sourceUser, key ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeK( theChan, polarity, sourceUser, key ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeO( Channel* theChan, ChannelUser* sourceUser,
	const xServer::opVectorType& opVector )
{
theChan->onModeO( opVector ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeO( theChan, sourceUser, opVector ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeO( theChan, sourceUser, opVector ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeV( Channel* theChan, ChannelUser* sourceUser,
	const xServer::voiceVectorType& voiceVector )
{
theChan->onModeV( voiceVector ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeV( theChan, sourceUser,
			voiceVector ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeV( theChan, sourceUser, voiceVector ) ;
	}
}

// Handle a channel mode change
// theChan is the channel on which the mode change occured
// polarity is true if the mode is being set, false otherwise
// sourceUser is the source of the mode change; this variable
// may be NULL if a server is setting the mode
void xServer::onChannelModeB( Channel* theChan, ChannelUser* sourceUser,
	xServer::banVectorType& banVector )
{

// Channel::onModeB() may modify banVector with the extra bans
// that have been removed due to overlaps
theChan->onModeB( banVector ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeB( theChan, sourceUser,
			banVector ) ;
		}
	}

// Find listeners for this specific channel
channelEventMapType::iterator chanPtr =
	channelEventMap.find( theChan->getName() ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

// Iterate through the listeners for this channel's events
// and notify each listener of the event
list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelModeB( theChan, sourceUser, banVector ) ;
	}
}

void xServer::removeAllChanModes( Channel* theChan )
{
// This is a protected method, theChan is non-NULL
if( theChan->getMode( Channel::MODE_T ) )
	{
	onChannelModeT( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_N ) )
	{
	onChannelModeN( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_S ) )
	{
	onChannelModeS( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_P ) )
	{
	onChannelModeP( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_M ) )
	{
	onChannelModeM( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_I ) )
	{
	onChannelModeI( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_L ) )
	{
	onChannelModeL( theChan, false, 0, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_K ) )
	{
	onChannelModeK( theChan, false, 0, string() ) ;
	}

opVectorType opVector ;
voiceVectorType voiceVector ;

for( Channel::userIterator ptr = theChan->userList_begin(),
	end = theChan->userList_end() ; ptr != end ; ++ptr )
	{
	if( ptr->second->getMode( ChannelUser::MODE_O ) )
		{
		opVector.push_back( opVectorType::value_type(
			false, ptr->second ) ) ;
		}
	if( ptr->second->getMode( ChannelUser::MODE_V ) )
		{
		voiceVector.push_back( voiceVectorType::value_type(
			false, ptr->second ) ) ;
		}
	}

banVectorType banVector ;

Channel::banIterator ptr = theChan->banList_begin(),
	end = theChan->banList_end() ;

for( ; ptr != end ; ++ptr )
	{
	banVector.push_back( banVectorType::value_type(
		false, *ptr ) ) ;
	}

if( !opVector.empty() )
	{
	onChannelModeO( theChan, 0, opVector ) ;
	}
if( !voiceVector.empty() )
	{
	onChannelModeV( theChan, 0, voiceVector ) ;
	}
if( !banVector.empty() )
	{
	onChannelModeB( theChan, 0, banVector ) ;
	}
}

void xServer::updateGlines()
{
time_t now = ::time( 0 ) ;

glineListType::iterator ptr = glineList.begin(),
	end = glineList.end() ;  
for( ; ptr != end ; )
	{
	if( (*ptr)->getExpiration() <= now )
		{
		// Expire the gline
		PostEvent( EVT_REMGLINE,
			static_cast< void* >( *ptr ) ) ;

		delete *ptr ;
		ptr = glineList.erase( ptr ) ;
		}
	else
		{
		++ptr ;
		}
	} // for()
} // updateGlines()

} // namespace gnuworld
