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

#include	<cstdlib>
#include	<cstdio>
#include	<cstdarg>
#include	<cstring>
#include	<cassert>
#include	<cerrno>

#include	"config.h"
#include	"misc.h"
#include	"events.h"
#include	"ip.h"

#include	"server.h"
#include	"Network.h"
#include	"EConfig.h"
#include	"Gline.h"

#include	"ELog.h"
#include	"MessageNode.h"
#include	"Socket.h"
#include	"StringTokenizer.h"
//#include	"moduleLoader.h"

const char xServer_h_rcsId[] = __XSERVER_H ;
const char xServer_cc_rcsId[] = "$Id: server.cc,v 1.20 2000/08/05 00:06:27 dan_karrels Exp $" ;

using std::string ;
using std::vector ;
using std::list ;
using std::endl ;
using std::ends ;
using std::strstream ;
using std::stack ;

namespace gnuworld
{

const string xServer::CHANNEL_ALL( "*" ) ;

/**
 * Instantiate a new xServer.  There is only one global
 * instance of this class.  The name of the server
 * configuration file is passed as argument.
 */
xServer::xServer( const string& _fileName )
 : eventList( EVT_NOOP )
{

// Read the configuration file and set our
// data members.
EConfig conf( _fileName ) ;

UplinkName = conf.Require( "uplink" )->second ;
ServerName = conf.Require( "name" )->second ;
ServerDescription = conf.Require( "description" )->second ;
Password = conf.Require( "password" )->second ;
Port = atoi( conf.Require( "port" )->second.c_str() ) ;
intYY = atoi( conf.Require( "numeric" )->second.c_str() ) ;
intXXX = atoi( conf.Require( "maxclients" )->second.c_str() ) ;

// Output the information to the console.
elog << "Numeric: " << intYY << endl ;
elog << "Max Clients (bogus): " << intXXX << endl ;
elog << "Uplink Name: " << UplinkName << endl ;  
elog << "Port: " << Port << endl ;
elog << "Server Name: " << ServerName << endl ;
elog << "Server Description: " << ServerDescription << endl ;

// Initialize more variables
bursting = false ;
_connected = false ;
StartTime = ::time( NULL ) ;

burstStart = burstEnd = 0 ;
Uplink = NULL ;
theSock = NULL ;
Message = SRV_SUCCESS ;
outputWriteSize = inputReadSize = 0 ;
lastTimerID = 1 ;

// Initialize the numeric stuff.
memset( charYY, 0, sizeof( charYY ) ) ;
memset( charXXX, 0, sizeof( charXXX ) ) ;

inttobase64( charYY, intYY, 2 ) ;
inttobase64( charXXX, intXXX, 3 ) ;

elog << "xServer::charYY> " << charYY << endl ;
elog << "xServer::charXXX> " << charXXX << endl ;
elog << "xServer::intYY> " << intYY << endl ;
elog << "xServer::intXXX> " << intXXX << endl ;

// Allocate the command map
try
	{
	// 36 is the number of valid characters in
	// the command map's key character set
	// They include: alpha chars (case insensitive,
	// so only 26 of these), numeric characters: 10
//	commandMap = new commandMapType( 36, serverCommandTransTable ) ;
	commandMap = new commandMapType ;
	}
catch( std::bad_alloc )
	{
	elog		<< "xServer()> Memory allocation failure\n" ;
	exit( 0 ) ;
	}

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
REGISTER_MSG( "VERSION", Version );

// Server
REGISTER_MSG( "S", S );
REGISTER_MSG( "SERVER", Server );

// Nick
REGISTER_MSG( "N", N );
REGISTER_MSG( "NICK", N );

// End of Burst
REGISTER_MSG( "END_OF_BURST", EndOfBurst );
REGISTER_MSG( "EB", EB );

// End of Burst Acknowledge
REGISTER_MSG( "EA", EA );
REGISTER_MSG( "EOB_ACK", EA );

// Ping
REGISTER_MSG( "G", G );
REGISTER_MSG( "PING", Ping );

// Privmsg
// Also register NOTICE to
// be handled by the privmsg
// command handler.
//
REGISTER_MSG( "O", P );
REGISTER_MSG( "P", P );
REGISTER_MSG( "NOTICE", P );
REGISTER_MSG( "PRIVMSG", P );

// Mode
REGISTER_MSG( "M", M );

// Quit
REGISTER_MSG( "Q", Q );
REGISTER_MSG( "QUIT", Quit );

// BURST
REGISTER_MSG( "B", B );
REGISTER_MSG( "BURST", B );

// Join
REGISTER_MSG( "J", J ) ;

// Create
REGISTER_MSG( "C", C ) ;
REGISTER_MSG( "CREATE", C ) ;

// Leave
REGISTER_MSG( "L", L ) ;

// Squit
REGISTER_MSG( "SQUIT", SQ ) ;
REGISTER_MSG( "SQ", SQ ) ;

// Kill
REGISTER_MSG( "D", D ) ;

REGISTER_MSG( "WA", WA ) ;

REGISTER_MSG( "R", R ) ;

// PASS
REGISTER_MSG( "PASS", PASS ) ;

// GLINE
REGISTER_MSG( "GL", GL ) ;
REGISTER_MSG( "GLINE", GL ) ;

// TOPIC
REGISTER_MSG( "T", T ) ;
REGISTER_MSG( "TOPIC", T ) ;

// KICK
REGISTER_MSG( "K", K ) ;
REGISTER_MSG( "KICK", K ) ;

// No idea
REGISTER_MSG( "DS", DS ) ;

// Admin
REGISTER_MSG( "AD", AD ) ;
REGISTER_MSG( "ADMIN", AD ) ;

// Non-tokenized command handlers

// Part
REGISTER_MSG( "PART", Part ) ;

// Join
REGISTER_MSG( "JOIN", Join ) ;

// Mode
REGISTER_MSG( "MODE", Mode ) ;

// Kill
REGISTER_MSG( "KILL", Kill ) ;

// WHOIS
REGISTER_MSG( "W", NOOP ) ;
REGISTER_MSG( "WHOIS", NOOP ) ;
REGISTER_MSG( "MOTD", NOOP ) ;
REGISTER_MSG( "MO", NOOP ) ; // MOTD
REGISTER_MSG( "STATS", NOOP ) ;
REGISTER_MSG( "V", NOOP ) ; // Version
REGISTER_MSG( "I", NOOP ) ; // Invite
REGISTER_MSG( "TR", NOOP ) ; // Trace
REGISTER_MSG( "SETTIME", NOOP ) ;
REGISTER_MSG( "368", NOOP ) ; // End of channel ban list

// AWAY
REGISTER_MSG( "A", NOOP ) ;
REGISTER_MSG( "AWAY", NOOP ) ;

// SILENCE
REGISTER_MSG( "SILENCE", NOOP ) ;

REGISTER_MSG( "441", NOOP ) ;

// DESYNCH
REGISTER_MSG( "DESYNCH", Desynch ) ;

/*
 * Load and attach any modules specified in the config.
 */ 
/*
EConfig::const_iterator ptr = conf.Find( "module" ) ;
for( ; ptr != conf.end() && ptr->first == "module" ; ++ptr )
	{
	StringTokenizer modInfo(ptr->second) ;
	elog	<< "xServer> Found module: " << modInfo[0]
		<< " (Config: " << modInfo[1] << ")" << endl;

	moduleLoader< xClient* >* ml =
		new (nothrow) moduleLoader< xClient* >( modInfo[ 0 ] ) ;
	if( NULL == ml )
		{
		elog << "xServer> Memory allocation failure loading modules\n" ;
		break ;
		}

	moduleList.push_back(ml); // Add moduleLoader to list. 
	xClient* clientPtr = ml->loadObject(modInfo[1]);
	if( NULL == clientPtr )
		{
		elog	<< "xServer> Failed to instantiate module: "
			<< modInfo[ 1 ] << endl ;
		continue ;
		}
 
	AttachClient(clientPtr); 
	}
*/
}

/**
 * Deallocate this xServer instance.
 */
xServer::~xServer()
{
// TODO: Delete all clients
// TODO: Delete all commands in command map
delete commandMap ;

// Deallocate all of the Glines
for( glineListType::iterator ptr = glineList.begin() ; ptr != glineList.end() ;
	++ptr )
	{
	delete *ptr ;
	}
glineList.clear() ;

/*
// Deallocate all loaded modules/close dlm handles.
for( moduleListType::iterator ptr = moduleList.begin() ; ptr != moduleList.end() ;
	++ptr )
	{
	delete *ptr ;
	}
moduleList.clear() ;
*/

while( !timerQueue.empty() )
	{
	delete timerQueue.top().second ;
	timerQueue.pop() ;
	}

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
	try
		{
		// Allocate the socket.
		theSock = new ClientSocket ;
		}
	catch( std::bad_alloc )
		{
		elog << "xServer::Connect> Memory allocation failure\n" ;
		return -1 ;
		}
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

// Notify the curious user of the TCP window sizes.
//elog << "inputReadSize: " << inputReadSize << endl ;
//elog << "outputWriteSize: " << outputWriteSize << endl ;

// Login to the uplink.
Write( "PASS :%s\n", Password.c_str() ) ;

// Send our server information.
Write( "SERVER %s %d %d %d J%02d %s :%s\n",
	        ServerName.c_str(),
		1,
		StartTime,
		ConnectionTime,
		Version,
		getCharYYXXX().c_str(),
		ServerDescription.c_str() ) ;


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

return 0 ;

}

void xServer::OnDisConnect()
{
if( theSock )
	{
	theSock->close() ;
	delete theSock ; theSock = 0 ;
	}

_connected = false ;
inputReadSize = outputWriteSize = 0 ;

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
bool xServer::RegisterChannelEvent( const string& chanName, xClient* theClient )
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
	const string& chanName,
	void* Data1, void* Data2,
	void* Data3, void* Data4 )
{

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr = channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelEvent( theEvent, chanName,
			Data1, Data2, Data3, Data4 ) ;
		}
	}

channelEventMapType::iterator chanPtr = channelEventMap.find( chanName ) ;
if( chanPtr == channelEventMap.end() )
	{
	// No listeners for this channel's events
	return ;
	}

list< xClient* >* listPtr = chanPtr->second ;
for( list< xClient* >::iterator ptr = listPtr->begin(), end = listPtr->end() ;
	ptr != end ; ++ptr )
	{
	(*ptr)->OnChannelEvent( theEvent, chanName,
		Data1, Data2, Data3, Data4 ) ;
	}
}

// Attempt a read from the network connection.
// Returns the number of bytes read or -1 on error.
int xServer::DoRead()
{

// Is there a valid connection?
if( !_connected )
	{
	return -1 ;
	}

// See how many bytes are ready to be read without blocking.
int bytesAvailable = theSock->readable() ;
if( bytesAvailable < 0 )
	{
	// Error on socket
	elog	<< "xServer::DoRead> Read error: "
		<< strerror( errno ) << endl ;
	OnDisConnect() ;
	return -1 ;
	}
else if( 0 == bytesAvailable )
	{
	// No data available, no big deal.
	return 0 ;
	}

// Allocate a small C buffer to receive the
// data from the socket.
//
char* inBuf = 0 ;
try
	{
	// inputReadSize is the size of the TCP receive window, and
	// is determined at runtime in Connect().
	inBuf = new char[ inputReadSize + 1 ] ;
	}
catch( std::bad_alloc )
	{
	// Unable to allocate memory.
	elog	<< "xServer::DoRead> Memory allocation failure\n" ;
	return -1 ;
	}

// Typecast here to make sure we get the correct recv method.
int bytesRead = theSock->recv(
	reinterpret_cast< unsigned char* >( inBuf ), inputReadSize + 1 ) ;

// Was the recv successful?
if( bytesRead < 0 )
	{
	// Nope, read error.  The connection is no
	// longer valid.

	OnDisConnect() ;

	// Don't forget to deallocate the temp buffer.
	delete[] inBuf ;

	elog	<< "xServer::DoRead> Got a read error: "
		<< strerror( errno ) << endl ;

	// Return error, errno is already set.
	return -1 ;
	}

// Null terminate the buffer we just read
inBuf[ bytesRead ] = 0 ;

// Append this buffer to the input buffer.
inputBuffer += inBuf ;

// Don't forget to deallocate the buffer.
delete[] inBuf ;

//elog << "xServer::DoRead> Read " << bytesRead << " bytes\n" ;

// Return the number of bytes read.
return bytesRead ;

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
int bytesWritten = theSock->send( outputBuffer.toString() ) ;

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
bool xServer::GetString( char* buf, const size_t& size )
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
bool xServer::Write( const string& buf )
{

// Is there a valid connection?
if( !_connected )
	{
	return false ;
	}

#ifdef EDEBUG
	// Output the debugging information
	// to the console.
	cout << "[OUT]: " << buf  ;

	// Should we output a trailing newline
	// character?
	if( buf[ buf.size() - 1 ] != '\n' )
		{
		cout << endl ;
		}
#endif

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
return true ;

}

/**
 * Write the contents of a std::strstream to the uplink connection.
 */
bool xServer::Write( strstream& s )
{
return Write( string( s.str() ) ) ;
}

/**
 * This method appends the variable sized argument
 * list buffer to the output buffer.
 * Returns false if no valid connection available,
 * true otherwise.
 * I despise this function. --dan
 */
bool xServer::Write( const char* format, ... )
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

// Return success.
return true ;

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
s	<< charYY << " GL * -" << userHost << ends ;
Write( s ) ;
delete[] s.str() ;

delete *ptr ;
glineList.erase( ptr ) ;

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

Gline* newGline = 0 ;
try
	{
	newGline = new Gline( setBy, userHost, reason, duration ) ;
	}
catch( std::bad_alloc )
	{
	elog	<< "xServer::setGline> Memory allocation failure\n" ;
	exit( 0 ) ;
	}

// Notify the rest of the network
strstream s ;
s	<< charYY << " GL * +"
	<< userHost << ' '
	<< duration << " :"
	<< reason << ends ;
Write( s ) ;
delete[] s.str() ;

glineList.push_back( newGline ) ;

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

// MSG_B
// This is the BURST command.
// 0 B #merida 957075177 +tn BAA:o
// 0 B #ValHalla 000031335 +stn CAA:o
// 0 B #linux 801203210 +stn OBL,LLf:ov,MBU:o,OBE,MAg,MAh
// 0 B #mylinux 953234759 +tn OBL,MBU:o,OBE,MAg,MAh
// 0 B #krushnet 000031337 +tn LEM,qAD,OAi,2B6,kA],kA9,qAA,2B2,NCR,kAD,OAC,0DK:o,MAL,zDj
// :%*!*lamer@*lamer.lamer.lamer.lamer11.com *!*lamer@*lamer.lamer.lamer.lamer10.com
// *!*lamer@*lamer.lamer.lamer.lamer9.com *!*lamer@*lamer.lamer.lamer.lamer8.com
// *!*lamer@*lamer.lamer.lamer.lamer7.com *!*lamer@*lamer.lamer.lamer.lamer6.com
// *!*lamer@*lamer.lamer.lamer.lamer5.com *!*lamer@*lamer.lamer.lamer.lamer4.com
// *!*lamer@*lamer.lamer.lamer.lamer3.com *!*lamer@*lamer.lamer.lamer.lamer2.com
//
// Q B #ateneo 848728923 +tnl 2000 r]Q,ZLC,Smt,rGN,gPk,uhy,Z]N,oTL,uem,31b,Znt,
//  3x3,oC0,TvC,3vs,oSo,IP7,oXL,aF2,CW9,sTq,Znw,Is9,gPD,rI1,ToI,ZZK,oGB,$Q
// B #ateneo 848728923 4Qt,LE2,LXJ,3ys,oIG,lwc,TQX,HwR,3iZ,g2D,ZP3,3m2,uPi,Z0n,
//  LTi,oG[,a3N,IH4,T3T,La],goY,geE,sar,oid,o90,35Y,TUL,Z7K,Zx7,TN1,C6$Q
// B #ateneo 848728923 :%*!*@203.145.226.149 *!*@203.177.4.* *!*@203.145.226.134
// *!*@202.8.230.*
//
// Q B #hgsd 933357379 +tn PIs,OfK,OAu,PZl:o,eAA
//
int xServer::MSG_B( xParameters& Param )
{
if( Param.size() < 4 )
	{
	elog	<< "xServer::MSG_B> Invalid number of arguments\n" ;
	return -1 ;
	}

Channel* theChan = Network->findChannel( Param[ 1 ] ) ;
if( NULL == theChan )
	{
	try
		{
		theChan = new Channel( Param[ 1 ], atoi( Param[ 2 ] ) ) ;
		}
	catch( std::bad_alloc )
		{
		elog	<< "xServer::MSG_B> Memory allocation failure\n" ;
		return -1 ;
		}
	if( !Network->addChannel( theChan ) )
		{
		elog	<< "xServer::MSG_B> Failed to add channel: "
			<< Param[ 1 ] << endl ;
		delete theChan ;
		return -1 ;
		}
	} // if( NULL == theChan )
else
	{
	time_t newCreationTime = static_cast< time_t >( ::atoi( Param[ 2 ] ) ) ;
	if( newCreationTime != theChan->getCreationTime() )
		{
		theChan->setCreationTime( newCreationTime ) ;
		// TODO: Clear channel modes etc?
		}
	}

// Parse out the channel state
xParameters::size_type whichToken = 3 ;

// Channel modes will always be the first thing to follow if it's in the burst
if( '+' == Param[ whichToken ][ 0 ] )
	{
	// channel modes
	const char* currentPtr = Param[ whichToken ] ;

	// Skip over the '+'
	++currentPtr ;

	for( ; currentPtr && *currentPtr ; ++currentPtr )
		{
		switch( *currentPtr )
			{
			case 't':
				theChan->setMode( Channel::MODE_T ) ;
				break ;
			case 'n':
				theChan->setMode( Channel::MODE_N ) ;
				break ;
			case 'm':
				theChan->setMode( Channel::MODE_M ) ;
				break ;
			case 'p':
				theChan->setMode( Channel::MODE_P ) ;
				break ;
			case 's':
				theChan->setMode( Channel::MODE_S ) ;
				break ;

			// With these next two, Im assuming that ircu has properly
			// formatted the bursts.
			// This could cause a seg fault or miss a user
			// that's on this channel if the mode l/k is without
			// argument.
			// TODO: fix this, not difficult
			case 'l':
				theChan->setLimit( ::atoi( Param[ whichToken + 1 ] ) ) ;
				whichToken++ ;
				break ;
			case 'k':
				theChan->setKey( Param[ whichToken + 1 ] ) ;
				whichToken++ ;
				break ;
			default:
				break ;
			} // switch

		} // for( currentPtr != endPtr )

	// Skip over the modes token
	whichToken++ ;

	} // if( '+' == Param[ whichToken ][ 0 ]

// Have we reached the end of this burst command?
if( whichToken >= Param.size() )
	{
	return 0 ;
	}

for( ; whichToken < Param.size() ; ++whichToken )
	{
	// Bans will always be the last thing burst, so no users
	// will be burst afterwards.  This is useful because xParameters
	// will only delimit tokens by ':', so the ban string is guaranteed
	// to be caught.
	if( '%' == Param[ whichToken ][ 0 ] )
		{
		// Channel bans
		// Be sure to skip over the '%'
		parseBurstBans( theChan, Param[ whichToken ] + 1 ) ;
		}
	else
		{
		// Userlist
		parseBurstUsers( theChan, Param[ whichToken ] ) ;
		}
	}
return 0 ;
}

// PIs,OfK,OAu,PZl:o,eAA
void xServer::parseBurstUsers( Channel* theChan, const char* theUsers )
{

//clog	<< "xServer::parseBurstUsers> Channel: " << theChan->getName()
//	<< ", users: " << theUsers << endl ;

// Parse out users and their modes
StringTokenizer st( theUsers, ',' ) ;

for( StringTokenizer::const_iterator ptr = st.begin() ; ptr != st.end() ;
	++ptr )
	{
	// Each token is of the form:
	// abc or abc:modes
	string::size_type pos = (*ptr).find_first_of( ':' ) ;

	iClient* theClient = Network->findClient( (*ptr).substr( 0, pos ) ) ;
	if( NULL == theClient )
		{
		elog	<< "xServer::parseBurstUsers> ("
			<< theChan->getName() << ")"
			<< ": Unable to find client: "
			<< (*ptr).substr( 0, pos ) << endl ;
		continue ;
		}

//	elog	<< "xServer::parseBurstUsers> Adding user " << theClient->getNickName()
//		<< "(" << theClient->getCharYYXXX() << ") to channel "
//		<< theChan->getName() << endl ;

	ChannelUser* chanUser = 0 ;
	try
		{
		chanUser = new ChannelUser( theClient ) ;
		}
	catch( std::bad_alloc )
		{
		elog	<< "xServer::parseBurstUsers> Memory allocation failure\n" ;

		// Fatal error
		exit( 0 ) ;
		}

	// Add this channel to the user's channel structure.
	theClient->addChannel( theChan ) ;

	// Add this user to the channel's database.
	if( !theChan->addUser( chanUser ) )
		{
		elog	<< "xServer::parseBurstUsers> Unable to add user "
			<< theClient->getNickName() << " to channel "
			<< theChan->getName() << endl ;
		delete chanUser ;
		continue ;
		}

	if( string::npos == pos )
		{
		// no ':' in this string, just add the user
		continue ;
		}

	// Otherwise, user modes have been specified.
	for( pos++ ; pos < (*ptr).size() ; ++pos )
		{
		switch( (*ptr)[ pos ] )
			{
			case 'o':
				chanUser->setMode( ChannelUser::MODE_O ) ;
				break ;
			case 'v':
				chanUser->setMode( ChannelUser::MODE_V ) ;
				break ;
			default:
				// TOOD: log
				break ;
			} // switch
		// for()
		}
	} // while( ptr != st.end() )
}

void xServer::parseBurstBans( Channel* theChan, const char* theBans )
{
//clog	<< "xServer::parseBurstBans> Found bans for channel "
//	<< theChan->getName() << ": " << theBans << endl ;

StringTokenizer st( theBans ) ;
for( StringTokenizer::size_type i = 0 ; i < st.size() ; ++i )
	{
	theChan->setBan( st[ i ] ) ;
	}
}

int xServer::MSG_Error( xParameters& )
{
return 0 ;
}

int xServer::MSG_PASS( xParameters& )
{
bursting = true ;
return 0 ;
}

// STATS
// kAB R O :AI
//
int xServer::MSG_R( xParameters& )
{
// TODO: Post message
return 0 ;
}

// WALLOPS
int xServer::MSG_WA( xParameters& )
{
// TODO: Post message
return 0;
}

/**
 * Someone has just left a channel.
 * AABBB L #channel
 */
int xServer::MSG_L( xParameters& Param )
{

if( Param.size() < 2 )
	{
	elog	<< "xServer::MSG_L> Invalid number of arguments\n" ;
	return -1 ;
	}

if( '+' == Param[ 1 ][ 0 ] )
	{
	// Don't care about modeless channels
	return 0 ;
	}

// Find the client in question
// TODO: This could conceivably find a matching client *sigh*
iClient* theClient = Network->findClient( Param[ 0 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_L> (" << Param[ 1 ]
		<< "): Unable to find client: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

StringTokenizer st( Param[ 1 ], ',' ) ;
for( StringTokenizer::size_type i = 0 ; i < st.size() ; ++i )
	{
	if( '+' == st[ i ][ 0 ] )
		{
		// Ignore modeless channels
		continue ;
		}

	// Get the channel that was just parted.
	Channel* theChan = Network->findChannel( st[ i ] ) ;
	if( NULL == theChan )
		{
		elog	<< "xServer::MSG_L> Unable to find channel: "
			<< st[ i ] << endl ;
		return -1 ;
		}

	// Remove and deallocate the ChannelUser instance from this
	// channel's ChannelUser structure.
	delete theChan->removeUser( theClient ) ;

	// Remove this channel from this client's channel structure.
	theClient->removeChannel( theChan ) ;

	// Post the event to the clients listening for events on this
	// channel, if any.
	// TODO: Update message posting
	// TODO: Check if channel is empty, remove if so

	PostChannelEvent( EVT_PART, theChan->getName(),
		static_cast< void* >( theChan ),
		static_cast< void* >( theClient ) ) ;

	if( theChan->empty() && !Network->servicesOnChannel( theChan ) )
		{
		// No users in the channel, remove it.
		delete Network->removeChannel( theChan->getName() ) ;
		}
	} // for

return 0 ;
}

/**
 * Non-tokenized command handler.
 */
int xServer::MSG_Part( xParameters& Param )
{

if( Param.size() < 2 )
	{
	elog	<< "xServer::MSG_Part> Invalid number of arguments\n" ;
	return -1 ;
	}

if( '+' == Param[ 1 ][ 0 ] )
	{
	// Don't care about modeless channels
	return 0 ;
	}

// Find the client in question
iClient* theClient = Network->findNick( Param[ 0 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_Part> ("
		<< Param[ 1 ] << ") Unable to find client: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

StringTokenizer st( Param[ 1 ], ',' ) ;
for( StringTokenizer::size_type i = 0 ; i < st.size() ; ++i )
	{
	// Get the channel that was just parted.
	Channel* theChan = Network->findChannel( st[ i ] ) ;
	if( NULL == theChan )
		{
		elog	<< "xServer::MSG_Part> Unable to find channel: "
			<< st[ i ] << endl ;
		return -1 ;
		}

	// Remove and deallocate the ChannelUser instance from this
	// channel's ChannelUser structure.
	delete theChan->removeUser( theClient ) ;

	// Remove this channel from this client's channel structure.
	theClient->removeChannel( theChan ) ;

	// Post the event to the clients listening for events on this
	// channel, if any.
	// TODO: Update message posting
	// TODO: Check if channel is empty, remove if so

	PostChannelEvent( EVT_PART, theChan->getName(),
		static_cast< void* >( theChan ),
		static_cast< void* >( theClient ) ) ;

	if( theChan->empty() && !Network->servicesOnChannel( theChan ) )
		{
		// No users in the channel, remove it.
		delete Network->removeChannel( theChan->getName() ) ;
		}
	} // for

return 0 ;
}

// AIAAA K #coder-com 0C] :This is now an IRCoperator only channel
// Note that when a user is kicked from a channel, the user is not
// actually parted.  A separate MSG_L message will be issued after
// the MSG_K().
//
int xServer::MSG_K( xParameters& Param )
{

if( Param.size() < 3 )
	{
	elog	<< "xServer::MSG_K> Invalid number of arguments\n" ;
	return -1 ;
	}

if( '+' == Param[ 1 ][ 0 ] )
	{
	// Don't care about modeless channels
	return 0 ;
	}

// Find the client in question.
iClient* theClient = Network->findClient( Param[ 2 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_K> ("
		<< Param[ 1 ] << ") Unable to find client: "
		<< Param[ 2 ] << endl ;
	return -1 ;
	}

// Find the channel in question.
Channel* theChan = Network->findChannel( Param[ 1 ] ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::MSG_K> Unable to find channel: "
		<< Param[ 1 ] << endl ;
	return -1 ;
	}

delete theChan->removeUser( theClient ) ;
theClient->removeChannel( theChan ) ;

// All we really have to do here is post the message.
// TODO: Send the source of the kick
PostChannelEvent( EVT_KICK, theChan->getName(),
	static_cast< void* >( theChan ),
	static_cast< void* >( theClient ) ) ;

// Any users left in the channel?
if( theChan->empty() && !Network->servicesOnChannel( theChan ) )
	{
	delete Network->removeChannel( theChan->getName() ) ;
	}

return 0 ;

}

/**
 * Someone has just joined an empty channel (create)
 * UAA C #xfactor 957134023
 * zBP C #OaXaCa,#UruApan,#skatos 957207634
 */
int xServer::MSG_C( xParameters& Param )
{

if( Param.size() < 3 )
	{
	elog	<< "xServer::MSG_C> Invalid number of parameters\n" ;
	return -1 ;
	}

if( '+' == Param[ 1 ][ 0 ] )
	{
	// Don't care about modeless channels
	return 0 ;
	}

// Find the client in question.
iClient* theClient = Network->findClient( Param[ 0 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_C> ("
		<< Param[ 1 ] << ") Unable to find client: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

// Grab the creation time.
time_t creationTime = static_cast< time_t >( atoi( Param[ Param.size() - 1 ] ) ) ;

// Tokenize based on ','.  Multiple channels may be put into the
// same C command.
StringTokenizer st( Param[ 1 ], ',' ) ;
for( StringTokenizer::const_iterator ptr = st.begin() ; ptr != st.end() ;
	++ptr )
	{

	// Find the channel in question.
	Channel* theChan = Network->findChannel( *ptr ) ;
	if( NULL == theChan )
		{
		// Channel doesn't exist, allocate it
		try
			{
			theChan = new Channel( *ptr, creationTime ) ;
			}
		catch( std::bad_alloc )
			{
			elog	<< "xServer::MSG_C> Memory allocation failure\n" ;
			return -1 ;
			}

		// Add it to the network table
		if( !Network->addChannel( theChan ) )
			{
			elog	<< "xServer::MSG_C> Failed to add channel: "
				<< *theChan << endl ;
			delete theChan ;
			return -1 ;
			}
		}

	// Create a new ChannelUser to represent this iClient's
	// membership in this channel.
	ChannelUser* theUser = 0 ;
	try
		{
		// Allocate the ChannelUser.
		theUser = new ChannelUser( theClient ) ;
		}
	catch( std::bad_alloc )
		{
		elog	<< "xServer::MSG_C> Memory allocation failure\n" ;
		exit( 0 ) ;
		}

	// The user who creates a channel is automatically +o
	theUser->setMode( ChannelUser::MODE_O ) ;

	// Build associations
	if( !theChan->addUser( theUser ) )
		{
		elog	<< "xServer::MSG_C> Unable to add user "
			<< theUser->getNickName() << " to channel "
			<< theChan->getName() << endl ;

		delete theUser ;
		return -1 ;
		}

	// Add this channel to the client's channel structure.
	theClient->addChannel( theChan ) ;

	PostChannelEvent( EVT_CREATE, theChan->getName(),
		static_cast< void* >( theChan ),
		static_cast< void* >( theClient ) ) ;

	} // for()

return 0 ;

}

/**
 * Someone has just joined a non-empty channel.
 *
 * 0AT J #coder-com
 * OAT J #coder-com,#blah
 */
int xServer::MSG_J( xParameters& Param )
{

if( Param.size() < 2 )
	{
	elog	<< "xServer::MSG_J> Invalid number of arguments\n" ;
	return -1 ;
	}

// Find the client in question.
iClient* Target = Network->findClient( Param[ 0 ] ) ;
if( NULL == Target )
	{
	elog	<< "xServer::MSG_J> ("
		<< Param[ 1 ] << ") Unable to find user: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

if( '0' == Param[ 1 ][ 0 ] )
	{
	// Artifact, user is parting all channels
	for( iClient::channelIterator ptr = Target->channels_begin(),
		endPtr = Target->channels_end() ; ptr != endPtr ; ++ptr )
		{
		PostChannelEvent( EVT_PART, (*ptr)->getName(),
			static_cast< void* >( *ptr ), // Channel*
			static_cast< void* >( Target ) ) ; // iClient*

		delete (*ptr)->removeUser( Target->getIntYY() ) ;
		if( (*ptr)->empty() && !Network->servicesOnChannel( *ptr ) )
			{
			delete Network->removeChannel( (*ptr)->getName() ) ;
			}
		}
	Target->clearChannels() ;
	return 0 ;
	}

// Tokenize by ',', as the client may join more than one
// channel at once.
StringTokenizer st( Param[ 1 ], ',' ) ;
for( StringTokenizer::size_type i = 0 ; i < st.size() ; i++ )
	{

	if( '+' == st[ i ][ 0 ] )
		{
		// Don't care about modeless channels
		continue ;
		}

	Channel* theChan = 0 ;
	ChannelUser* theUser = 0 ;
	try
		{
		theUser = new ChannelUser( Target ) ;
		}
	catch( std::bad_alloc )
		{
		elog	<< "xServer::MSG_J> Memory allocation failure\n" ;
		return -1 ;
		}

	channelEventType whichEvent = EVT_JOIN ;

	// On a JOIN command, the channel should already exist.
	theChan = Network->findChannel( st[ i ] ) ;
	if( NULL == theChan )
		{
		// This transmutes to a CREATE
		try
			{
			theChan = new Channel( st[ i ], ::time( 0 ) ) ;
			}
		catch( std::bad_alloc )
			{
			elog	<< "xServer::MSG_J> Memory allocation "
				<< "failure\n" ;
			delete theChan ;
			delete theUser ;
			return -1 ;
			}

		// Add the channel to the network tables
		if( !Network->addChannel( theChan ) )
			{
			elog	<< "xServer::MSG_J> Unable to add channel: "
				<< theChan->getName() << endl ;
			delete theChan ;
			delete theUser ;
			continue ;
			}

		// Since this is equivalent to a CREATE, set the user
		// as operator.
		theUser->setMode( ChannelUser::MODE_O ) ;

		whichEvent = EVT_CREATE ;
		} // if( NULL == theChan )

	// Otherwise, the channel was found just fine :)

	// Add a new ChannelUser representing this client to this
	// channel's user structure.
	if( !theChan->addUser( theUser ) )
		{
//		elog	<< "xServer::MSG_J> Unable to add user "
//			<< theUser->getNickName() << " to channel: "
//			<< theChan->getName() << endl ;

		// Addition of ChannelUser failed.
		delete theUser ;
		return -1 ;
		}

	// Add this channel to this client's channel structure.
	Target->addChannel( theChan ) ;

	// Post the event to the clients listening for events on this
	// channel, if any.
	PostChannelEvent( whichEvent, theChan->getName(),
		static_cast< void* >( theChan ),
		static_cast< void* >( Target ) ) ;

	} // for()

return 0 ;

}

// Non-tokenized command handler
//
// nickname JOIN #channel timestamp
int xServer::MSG_Join( xParameters& Param )
{

if( Param.size() < 2 )
	{
	elog	<< "xServer::MSG_Join> Invalid number of arguments\n" ;
	return -1 ;
	}

// Find the client in question.
iClient* Target = Network->findNick( Param[ 0 ] ) ;
if( NULL == Target )
	{
	elog	<< "xServer::MSG_Join> ("
		<< Param[ 1 ] << ") Unable to find user: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

if( '0' == Param[ 1 ][ 0 ] )
	{
	// Artifact, user is parting all channels
	for( iClient::channelIterator ptr = Target->channels_begin(),
		endPtr = Target->channels_end() ; ptr != endPtr ; ++ptr )
		{
		delete (*ptr)->removeUser( Target->getIntYY() ) ;
		if( (*ptr)->empty() && !Network->servicesOnChannel( *ptr ) )
			{
			delete Network->removeChannel( (*ptr)->getName() ) ;
			}
		}
	Target->clearChannels() ;
	return 0 ;
	}

// Tokenize by ',', as the client may join more than one
// channel at once.
StringTokenizer st( Param[ 1 ], ',' ) ;
for( StringTokenizer::size_type i = 0 ; i < st.size() ; i++ )
	{

	if( '+' == st[ i ][ 0 ] )
		{
		// Don't care about modeless channels
		continue ;
		}

	Channel* theChan = 0 ;
	ChannelUser* theUser = 0 ;
	try
		{
		theUser = new ChannelUser( Target ) ;
		}
	catch( std::bad_alloc )
		{
		elog	<< "xServer::MSG_Join> Memory allocation failure\n" ;
		return -1 ;
		}

	// On a JOIN command, the channel should already exist.
	theChan = Network->findChannel( st[ i ] ) ;
	if( NULL == theChan )
		{
		// This transmutes to a CREATE
		try
			{
			theChan = new Channel( st[ i ], ::time( 0 ) ) ;
			}
		catch( std::bad_alloc )
			{
			elog	<< "xServer::MSG_Join> Memory allocation "
				<< "failure\n" ;
			delete theChan ;
			delete theUser ;
			return -1 ;
			}

		// Add the channel to the network tables
		if( !Network->addChannel( theChan ) )
			{
			elog	<< "xServer::MSG_Join> Unable to add channel: "
				<< theChan->getName() << endl ;
			delete theChan ;
			delete theUser ;
			continue ;
			}

		// Since this is equivalent to a CREATE, set the user
		// as operator.
		theUser->setMode( ChannelUser::MODE_O ) ;

		} // if( NULL == theChan )

	// Otherwise, the channel was found just fine :)

	// Add a new ChannelUser representing this client to this
	// channel's user structure.
	if( !theChan->addUser( theUser ) )
		{
//		elog	<< "xServer::MSG_Join> User "
//			<< theUser->getNickName() << " already on channel "
//			<< theChan->getName() << endl ;

		// Addition of ChannelUser failed.
		delete theUser ;
		return -1 ;
		}

	// Add this channel to this client's channel structure.
	Target->addChannel( theChan ) ;

	// Post the event to the clients listening for events on this
	// channel, if any.
	PostChannelEvent( EVT_JOIN, theChan->getName(),
		static_cast< void* >( theChan ),
		static_cast< void* >( Target ) ) ;

	} // for()

return 0 ;

}

// A nick has requested a version
int xServer::MSG_Version( xParameters& Param )
{
return 0 ;
}

// WHOIS
//
int xServer::MSG_NOOP( xParameters& Param )
{
return 0 ;
}

/**
 * Kill command
 * QAA D BB5 :localhost!_reppir (Im using my super duper clone detecting
 *  skills)
 * G D r[l :NewYork-R.NY.US.Undernet.Org!NewYork-R.NY.US.Undernet.org ...
 * The source of the kill could be a server or a client.
 */
int xServer::MSG_D( xParameters& Param )
{

// See if the client being killed is one of my own.
xClient* myClient = Network->findLocalClient( Param[ 1 ] ) ;

// Is the user being killed on this server?
if( NULL != myClient )
	{
	// doh, yes it is :(
	myClient->OnKill() ;

	// Don't detach the client until it requests so.
	// TODO: Work on this system.

	// Note that the client is still attached to the
	// server.
	return 0 ;
	}

// Otherwise, it's a non-local client.
iClient* source = 0 ;
iServer* serverSource = 0 ;

if( strchr( Param[ 0 ], '.' ) != NULL )
	{
	// Server, by name
	serverSource = Network->findServerName( Param[ 0 ] ) ;
	}
else if( strlen( Param[ 0 ] ) >= 3 )
	{
	// Client, by numeric
	source = Network->findClient( Param[ 0 ] ) ;
	}
else
	{
	// Server, by numeric
	serverSource = Network->findServer( Param[ 0 ] ) ;
	}

if( (NULL == serverSource) && (NULL == source) )
	{
	elog	<< "xServer::MSG_D> Unable to find source: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

// Find and remove the client that was just killed.
// xNetwork::removeClient will remove user<->channel associations
iClient* target = Network->removeClient( Param[ 1 ] ) ;

// Make sure we have valid pointers to both source
// and target.
if( NULL == target )
	{
	elog	<< "xServer::MSG_D> Unable to find target client: "
		<< Param[ 1 ] << endl ;
	return -1 ;
	}

// Notify all listeners of the EVT_KILL event.
string reason( Param[ 2 ] ) ;

if( source != NULL )
	{
	PostEvent( EVT_KILL,
		static_cast< void* >( source ),
		static_cast< void* >( target ),
		static_cast< void* >( &reason ) ) ;
	}
else
	{
	PostEvent( EVT_KILL,
		static_cast< void* >( serverSource ),
		static_cast< void* >( target ),
		static_cast< void* >( &reason ) ) ;
	}

// Deallocate the memory associated with this iClient.
delete target ;

return 0 ;

}

// :pokebonk KILL LG[
// :NewYork-R.NY.US.Undernet.org!SantaClara.CA.US.Undernet.Org!dallas.tx.us.undernet.org
// !iago.nac.net!pokebonk (not here you don't)
//
// z KILL R[t :Baltimore-R.MD.US.Undernet.Org (NewYork-R.NY.US.Undernet.org <-
// austin.tx.us.undernet.org (Nick collision))
//
int xServer::MSG_Kill( xParameters& Param )
{

// See if the client being killed is one of my own.
xClient* myClient = Network->findLocalClient( Param[ 1 ] ) ;

// Is the user being killed on this server?
if( NULL != myClient )
	{
	// doh, yes it is :(
	myClient->OnKill() ;

	// Don't detach the client until it requests so.
	// TODO: Work on this system.

	// Note that the client is still attached to the
	// server.
	return 0 ;
	}

// Otherwise, it's a non-local client.
iClient* source = 0 ;
iServer* serverSource = 0 ;

if( strchr( Param[ 0 ], '.' ) != NULL )
	{
	// Server, by name
	serverSource = Network->findServerName( Param[ 0 ] ) ;
	}
else
	{
	// Nickname
	source = Network->findNick( Param[ 0 ] ) ;
	}

if( (NULL == source) && (NULL == serverSource) )
	{
	elog	<< "xServer::MSG_Kill> Unable to find source: "
		<< Param[ 0 ] << endl ;
//	The source isn't all that important
//	return -1 ;
	}

// Find and remove the client that was just killed.
// xNetwork::removeClient will remove user<->channel associations
iClient* target = Network->removeClient( Param[ 1 ] ) ;

// Make sure we have valid pointers to both source
// and target.
if( NULL == target )
	{
	elog	<< "xServer::MSG_D> Unable to find target client: "
		<< Param[ 1 ] << endl ;
	return -1 ;
	}

// Notify all listeners of the EVT_KILL event.
string reason( Param[ 2 ] ) ;

if( source != NULL )
	{
	PostEvent( EVT_KILL,
		static_cast< void* >( source ),
		static_cast< void* >( target ),
		static_cast< void* >( &reason ) ) ;
	}
else
	{
	PostEvent( EVT_KILL,
		static_cast< void* >( serverSource ),
		static_cast< void* >( target ),
		static_cast< void* >( &reason ) ) ;
	}

// Deallocate the memory associated with this iClient.
delete target ;

return 0 ;

}

/**
 * A nick has sent a private message
 * QBg P PAA :help
 * QBg: Source nickname's numeric
 * P: PRIVMSG
 * PAA: Destination nickname's numeric
 * :help: Message
 *
 * QAE P PAA :translate xaa
 * QAE P AAPAA :translate xaa
 */
int xServer::MSG_P( xParameters& Param )
{

char* Sender	= Param[ 0 ] ;
char* Receiver	= Param[ 1 ] ;

// Is the PRIVMSG being sent to a channel?
if( *Receiver == '#' )
	{
	// It's a channel message, just ignore it
	return 0 ;
	}

char		*Server		= NULL,
		*Pos		= NULL,
		*Command	= NULL ;

bool		CTCP		= false ;

xClient		*Client		= NULL ;

// Search for user@host in the receiver string
Pos = strchr( Receiver, '@' ) ;

// Was there a '@' in the Receiver string?
if( NULL != Pos )
	{
	// Yup, nickname specified
	Server = Receiver + (Pos - Receiver) + 1 ;
	Receiver[ Pos - Receiver ] = 0 ;
	Client = Network->findLocalClient( Receiver ) ;
	}
else if( Receiver[ 0 ] == charYY[ 0 ]
	&& Receiver[ 1 ] == charYY[ 1 ] )
	{
	// It's mine
	Client = Network->findLocalClient( Receiver ) ;
	}
else
	{
	return -1 ;
	}

char* Message = Param[ 2 ] ;

// Is it a CTCP message?
if( Message[ 0 ] == 1 && Message[ strlen( Message ) - 1 ] == 1 )
	{
	Message++ ;
	CTCP = true ;
	Message[ strlen( Message ) - 1 ] = 0 ;

	// TODO: Get rid of this hideous method call
	// strtok() is a pos
	Command = strtok( Message, " " ) ;
	char* Msg = strtok( NULL, "\r" ) ; 
	Message = Msg ;

	// Message == 0 will cause std::string() constructor to crash.
	if( NULL == Message )
		{
		Message = "" ;
		}
	}

// :Sender PRIVMSG YXX :Message
// :Sender PRIVMSG YXX :\001Command\001
// :Sender PRIVMSG YXX :\001\Command\001 Message

if( NULL == Client )
	{
	elog	<< "xServer::MSG_P: Local client not found: "
		<< Receiver << endl ;
	return -1 ;
	}

iClient* Target = Network->findClient( Sender ) ;
if( NULL == Target )
	{
	elog	<< "xServer::MSG_P> Unable to find Sender: " << Sender << endl ;
	return -1 ;
	}

if( CTCP )
	{
	return Client->OnCTCP( Target, Command, Message ) ;
	}
else
	{
	return Client->OnPrivateMessage( Target, Message ) ;
	}
}

/**
 * New server message
 * SERVER irc-r.mediabit.net 1 934191376 946934706 J10 BD] :MCS MEDIABIT Testnet
 * Routing Server, Padova, Italy
 * SERVER Austin-R.TX.US.KrushNet.Org 1 900000000 958147697 J10 1]] :Large Penis
 *  Support Group IRC Server
 *
 * The following command has been changed to token S
 * B SERVER irc.mediabit.net 2 0 945972199 P10 CD] 0 :[193.76.114.11]
 * [193.76.114.11] MCS MEDIABIT Testnet Server, Padov
 *
 * Declaration of our own server:
 * SERVER ripper.ufl.edu 1 933022556 948162945 J10 QD] :[128.227.184.152]
 * University of Florida
 *
 * Remember that the "SERVER" parameter is removed.
 */
int xServer::MSG_Server( xParameters& Param )
{

burstEnd = 0 ;
burstStart = ::time( 0 ) ;

// Check the hopcount
// 1: It's our uplink
if( Param[ 1 ][ 0 ] == '1' )
	{

//	clog	<< "xServer::MSG_Server> Got Uplink: " << Param[ 0 ] << endl ;

	// It's our uplink
	if( Param.size() < 6 )
		{
		elog	<< "xServer::MSG_Server> Invalid number of parameters\n" ;
		return -1 ;
		}

	// Here's the deal:
	// We are just connecting to the network
	// We have just received the first server command,
	// telling us who our uplink server is.
	// We need to add our uplink to network tables.

	// Assume 3 character numerics
	unsigned int uplinkYY = convert2n[ Param[ 5 ][ 0 ] ] ;

	// Check for n2k, 5 character numerics
	if( strlen( Param[ 5 ] ) == 5 )
		{
		// n2k
		uplinkYY = base64toint( Param[ 5 ], 2 ) ;
		}

	iServer* me = 0 ;
	try
		{
		// Our uplink has its own numeric as its uplinkIntYY.
		Uplink = new iServer( 
			uplinkYY,
			Param[ 5 ], // yyxxx
			Param[ 0 ], // name
			atoi( Param[ 3 ] ), // connect time
			atoi( Param[ 2 ] ), // start time
			atoi( Param[ 4 ] + 1 ) ) ; // version
		me = new iServer(
			Uplink->getIntYY(),
			getCharYYXXX(),
			ServerName,
			ConnectionTime,
			StartTime,
			Version ) ;
		}
	catch( std::bad_alloc )
		{

		// Prevent a memory leak, Uplink may have been allocated,
		// and me failed.
		delete Uplink ;

		// Set both pointer to 0.
		Uplink = me = 0 ;

		elog	<< "xServer::MSG_Server> Memory allocation failure\n" ;

		return -1 ;
		}

	// We now have a pointer to our own uplink
	// Add it to the tables
	// We maintain a local pointer just for speed reasons
	Network->addServer( Uplink ) ;
	Network->addServer( me ) ;

//	elog << "Added server: " << *Uplink ;
//	elog << "Added server: " << *me ;

	// We just connected, begin our BURST
//	Burst() ;

	}
else
	{
	// A server joining the network.
	// This is deprecated with the introduction
	// of tokenized commands.

//	elog << "MSG_SERVER: Depracated section\n" ;
//	clog	<< "xServer::MSG_Server> Param[ 3 ]: " << Param[ 3 ] << endl ;

	const char* ServerName = Param[ 1 ] ;
	time_t StartTime = atoi( Param[ 3 ] ) ;
	time_t ConnectionTime = atoi( Param[ 4 ] ) ;
	const char* Version = Param[ 5 ];
	const char* YXX = Param[ 6 ] ;

	iServer* uplink = Network->findServer( Param[ 0 ] ) ;
	if( NULL == uplink )
		{
		elog	<< "xServer::MSG_Server> Unable to find server: "
			<< Param[ 0 ] << endl ;
		return -1 ;
		}

	unsigned int intYY = convert2n[ YXX[ 0 ] ] ;
	if( strlen( YXX ) == 5 )
		{
		// n2k
		intYY = base64toint( YXX, 2 ) ;
		}

	if( *Version == 'J' || *Version == 'P' )
		{
		iServer* newServer = 0 ;
		try
			{
			newServer = new iServer(
				uplink->getIntYY(),
				YXX,
				ServerName,
				ConnectionTime,
				StartTime,
				atoi( Version ) ) ;
			}
		catch( std::bad_alloc )
			{
			elog	<< "MSG_Server> Memory allocation failure\n" ;
			return -1 ;
			}

		Network->addServer( newServer ) ;
		}

	if( !bursting && *Version == 'J' )
		{
		iServer* Server = Network->findServer( ServerName ) ;
		iServer* Uplink = Network->findServer( Param[ 0 ] ) ;
		if( Server && Uplink )
			{
			PostEvent( EVT_NETJOIN, Uplink, Server ) ;
			}
		}
	}
// Not posting message here because this method is only called once
// using tokenized commands - when the xServer connects
return 0 ;
}

/**
 * A client has quit.
 * QAE Q :Signed off
 */
int xServer::MSG_Q( xParameters& Param )
{

// xNetwork::removeClient will remove user<->channel associations
iClient* theClient = Network->removeClient( Param[ 0 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_Q> Unable to find client: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

PostEvent( EVT_QUIT, static_cast< void* >( theClient ) ) ;

// xNetwork::removeClient() will remove channel->user associations.
delete theClient ;

return 0 ;
}

/**
 * A quit message has been received
 * :_reppir QUIT :You are an imperfect being, created by an imperfect
 *  being.
 */
int xServer::MSG_Quit( xParameters& Param )
{
iClient* Who = Network->findNick( Param[ 0 ] ) ;
if( NULL == Who )
	{
	// BAD
	elog	<< "xServer::MSG_Quit> Unable to find nick: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

PostEvent( EVT_QUIT, static_cast< void* >( Who ) ) ;

delete Network->removeClient( Who->getIntYY(), Who->getIntXXX() ) ;

return( 0 ) ;
}

/**
 * SQUIT message handler.
 * :ripper_ SQ ripper.ufl.edu 0 :squitting the services server
 * :ripper_ SQ maniac.krushnet.org 954445164 :remote server squit
 * 0 SQ Auckland.NZ.KrushNet.Org 957468458 :Ping timeout
 * kAm SQ Luxembourg.LU.EU.KrushNet.Org 957423889 :byyyyyye
 *
 * The following example is gnuworld being squitted:
 * OAO SQ Asheville-R.NC.US.KrushNet.Org 0 :I bid you adoo
 * asheville-r.* was gnuworld's uplink
 * 0 SQ Asheville-R.NC.US.KrushNet.Org 0 :Ping timeout
 * Az SQ Seattle-R.WA.US.KrushNet.Org 0 :Ping timeout
 */
int xServer::MSG_SQ( xParameters& Param )
{
// Few things to do:
// - Clean user and server tables (remote only)
// - Clear input/output buffers
// - Delete socket connection, reinstantiate, reconnect

iServer* squitServer = 0 ;
if( strchr( Param[ 1 ], '.' ) != NULL )
	{
	// Full server name specified
	squitServer = Network->findServerName( Param[ 1 ] ) ;
	}
else
	{
	// Numeric
	squitServer = Network->findServer( Param[ 1 ] ) ;
	}

if( NULL == squitServer )
	{
	elog	<< "xServer::MSG_SQ> Unable to find server: "
		<< Param[ 1 ] << endl ;
	return -1 ;
	}

if( squitServer->getIntYY() == Uplink->getIntYY() )
	{
	elog	<< "xServer::MSG_SQ> Ive been delinked!!\n" ;

	// It's my uplink, we have been squit...those bastards!
	OnDisConnect() ;
	}
else
	{

//	elog	<< "xServer::MSG_SQ> " << squitServer->getName()
//		<< " has been squit\n" ;

	// Otherwise, it's just some server.
	Network->OnSplit( squitServer->getIntYY() ) ;

	// Remove this server from the network tables.
	Network->removeServer( squitServer->getIntYY() ) ;

	}

string source( Param[ 0 ] ) ;
string reason( Param[ 3 ] ) ;

PostEvent( EVT_NETBREAK,
	static_cast< void* >( squitServer ),
	static_cast< void* >( &source ),
	static_cast< void* >( &reason ) ) ;

// Deallocate
delete squitServer ;

return 0 ;
}

/**
 * Write an xClient channel part to the network, and update
 * network tables.
 */
void xServer::PartChannel( xClient* theClient, const string& chanName )
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

PartChannel( theClient, theChan ) ;
}

/**
 * Write an xClient channel part to the network, and update
 * network tables.
 */
void xServer::PartChannel( xClient* theClient, Channel* theChan )
{
#ifndef NDEBUG
  assert( theClient != NULL && theChan != NULL ) ;
#endif

strstream s ;
s	<< theClient->getCharYYXXX() << " L "
	<< theChan->getName() << ends ;

Write( s ) ;
delete[] s.str() ;

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
  assert( (theClient != NULL) && (theChan != NULL) ) ;
#endif

theClient->removeChannel( theChan ) ;
delete theChan->removeUser( theClient ) ;

// TODO: Post message

if( theChan->empty() && !Network->servicesOnChannel( theChan ) )
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

// TODO: post message
// TODO: Check for empty channel
// Let the other xClient's know that one of their own
// has parted a channel.

}

// TODO: This method is a bit ugly.
void xServer::JoinChannel( xClient* theClient, const string& chanName,
	const string& chanModes )
{

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	// Channel doesn't exist yet.
	// 0AT C #lksjhdlksjdlkjs 957214787
//	elog	<< "xServer::BurstChannel> Creating new channel: "
//		<< chanName << endl ;

	// Create the channel

	{
		strstream s ;
		s	<< theClient->getCharYYXXX() << " C "
			<< chanName << ' ' << time( 0 ) ;

		if( !chanModes.empty() )
			{
			s	<< ' ' << chanModes ;
			}
		s	<< ends ;

		Write( s ) ;
		delete[] s.str() ;
	}

	// Instantiate the new channel
	theChan = new Channel( chanName, time( 0 ) ) ;

	// Add it to the network channel table
	Network->addChannel( theChan ) ;

	}
else if( bursting )
	{
	// The channel already exists, burst the client into the channel.
	// Here is where we will actually use the Channel::creationTime :)
	// 0 B #coder-com 000031337 +tn 0AT,EAA:o,KAB,0AA
	strstream s ;
	s	<< getCharYY() << " B " << chanName << ' '
		<< theChan->getCreationTime() << ' '
		<< chanModes << ' '
		<< theClient->getCharYYXXX() << ":o"
		<< ends ;
	Write( s ) ;
	delete[] s.str() ;

	}
else
	{
	// After bursting, just join the channel and give the client
	// ops.
	{
		strstream s2 ;
		s2	<< theClient->getCharYYXXX() << " J "
			<< chanName << ends ;
		Write( s2 ) ;
		delete[] s2.str() ;
	}

	// Op the bot
	strstream s ;
	s	<< charYY << " M " << chanName
		<< " +o " << theClient->getCharYYXXX()
		<< ends ;
	Write( s ) ;
	delete[] s.str() ;

	}

}

void xServer::SetChannelMode( Channel* theChan, const string& theModes )
{
strstream s ;
s	<< getCharYYXXX() << " M " << theChan->getName()
	<< ' ' << theModes << ends ;
Write( s ) ;
delete[] s.str() ;

// TODO: Update modes in channel table
// theChan->OnModeChange()
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

void xServer::ProcessMessageQueue() 
{
// TODO: Who's to say that the clients are in
// continugous memory in the list?  Again, move
// to dynamic structure
/*
for( int i = 0 ; i < Clients ; i++ )
	{
	if( NULL == ClientList[ i ] )
		{
		continue ;
		}
	ClientList[ i ]->ProcessMessageQueue() ;
	} // close for loop
*/
}

// Received an end of burst message
// B END_OF_BURST
// Depracated method
int xServer::MSG_EndOfBurst( xParameters& Param )
{
if( NULL == theSock )
	{
	return -1 ;
	}

if( !strcmp( Param[ 0 ], Uplink->getCharYY() ) )
	{
	// It's my uplink
	burstEnd = ::time( 0 ) ;
	Write( "%s END_OF_BURST\n", charYY ) ;
	Write( "%s EOB_ACK\n", charYY ) ;
	}
if( !bursting )
	{
	iServer* theServer = Network->findServer( Param[ 0 ] ) ;
	if( NULL == theServer )
		{
		elog	<< "xServer::MSG_EndOfBurst> Unable to find server: "
			<< Param[ 0 ] << endl ;
		return -1 ;
		}

	PostEvent( EVT_BURST_CMPLT, static_cast< void* >( theServer ) ) ;
	}
return 0 ;
}

/**
 * A new user joined the network, or a user changed its nickname.
 * C NICK ripper_ 2 946934327 ~dan dolphinus.astro.ufl.edu +w CA47iY CAB
 *  :*Unknown*
 * CAB NICK _reppir 946934954
 *
 * If this method is called, then the uplink at least isn't using
 * tokenized commands.
 */
int xServer::MSG_Nick( xParameters& Param )
{

// Is this just a nickname change?
if( Param.size() < 8 )
	{
	Network->rehashNick( Param[ 0 ], Param[ 1 ] ) ;
	return 0 ;
	}

iServer* Server = Network->findServer( Param[ 0 ] ) ;
if( NULL == Server )
	{
	elog	<< "xServer::MSG_Nick> Unable to find server, "
		<< "Y: " << Param[ 0 ] << ", Numeric: "
		<< base64toint( Param[ 0 ] ) <<  endl ;
	return -1 ;
	}

const char	*Nick = 0,
		*YXX = 0,
		*CTime = 0,
		*UserID = 0,
		*Mode = 0,
		*Host = 0 ;

if( '+' == Param[ 6 ][ 0 ] )
	{
	// Mode set
	Mode = Param[ 6 ] ;
	Host = Param[ 7 ] ;
	YXX = Param[ 8 ] ;
	}
else
	{
	Host = Param[ 6 ] ;
	YXX = Param[ 7 ] ;
	Mode = "";
	}

const char* InsecureHostMask = Param[ 5 ] ;
Nick = Param[ 1 ] ;
CTime = Param[ 3 ] ;
UserID = Param[ 4 ] ;

AddUser( Server->getIntYY(),
	YXX,
	Nick,
	UserID,
	Host,
	InsecureHostMask,
	Mode,
	atoi( CTime ) ) ;

return 0 ;

}

/**
 * A new user has joined the network, or a user has changed
 * its nickname.
 * O N EUworld1 2 000201527 gnuworld1 undernet.org AAAAAA OAA :P10 Undernet
 * EUworld Service
 *
 * O: another server numeric
 * 2: hopcount
 * 000201527: timestamp
 * gnuworld1: username
 * undernet.org: domain
 * AAAAAA: base64 IP
 * OAA: numnick
 * :P10 Undernet: description
 *
 * B N hektik 2 948677656 hektik p62-max7.ham.ihug.co.nz +i DLbcbC BAA
 * :DiMeBoX ProduXiiions
 *
 * AU N Gte2 3 949526996 Gte 212.49.240.147 DUMfCT AUAAB :I am the one
 *  that was.
 */
int xServer::MSG_N( xParameters& params )
{

// AUAAB N Gte- 949527071
if( params.size() < 5 )
	{
	// User changing nick
	Network->rehashNick( params[ 0 ], params[ 1 ] ) ;
	return 0 ;
	}

// Else, it's the network giving us a new client.
iServer* nickUplink = Network->findServer( params[ 0 ] ) ;
if( NULL == nickUplink )
	{
	elog	<< "xServer::MSG_N> Unable to find server: "
		<< params[ 0 ] << endl ;
	return -1 ;
	}

// Default arguments, assuming
// no modes set.
const char* modes = "+" ;
const char* host = params[ 6 ] ;
const char* yyxxx = params[ 7 ] ;

// Are modes specified?
if( params.size() > 9 )
	{
	// Yup, all trailing fields
	// are offset by one.
	modes = params[ 6 ] ;
	host = params[ 7 ] ;
	yyxxx = params[ 8 ] ;
	}

iClient* newClient = 0 ;
try
	{
	newClient = new iClient(
		nickUplink->getIntYY(),
		yyxxx,
		params[ 1 ], // nickname
		params[ 4 ], // username
		host, // base 64 host
		params[ 5 ], // insecurehost
		modes,
		atoi( params[ 3 ] ) // connection time
		) ;
	}
catch( std::bad_alloc )
	{
	elog	<< "xServer::MSG_N> Memory allocation failure\n" ;
	return -1 ;
	}

//elog << "Adding client: " << *newClient ;
if( !Network->addClient( newClient ) )
	{
	elog	<< "xServer::MSG_B> Failed to add client: "
		<< *newClient << ", user already exists? "
		<< (Network->findClient( newClient->getCharYYXXX() ) ?
		   "yes" : "no") << endl ;
	delete newClient ;
	return -1 ;
	}

// TODO: Should this be posted?
PostEvent( EVT_NICK, static_cast< void* >( newClient ) ) ;

return 0 ;

}

/**
 * New server joined the network.
 * Q S irc.dynmc.net 2 0 948159347 P10 BD] 0 :[209.0.37.10]
 * [209.0.37.10] Dynamic Networking Solutions
 *
 * B S EUWorld1.test.net 3 0 947284938 P10 OD] 0 :[128.227.184.152]
 * EUWorld Undernet Service
 * B: Uplink Server numeric
 * S: SERVER message
 * EUWorld1.test.net: server name
 * 3: hopcount
 * 0: creation time
 * 947284938: link time
 * P10: Protocol
 * B: Server numeric
 * D]: Last used nick number for clients
 * 0: Unused
 * EUWorld Undernet Server: description
 * As always, the second token, the command, is not
 * included in the xParameters passed here.
 */
int xServer::MSG_S( xParameters& params )
{

// We need at least 9 tokens
if( params.size() < 9 )
	{
	elog	<< "xServer::MSG_S> Not enough parameters\n" ;
	return -1 ;
	}

int uplinkIntYY = base64toint( params[ 0 ] ) ;
iServer* uplinkServer = Network->findServer( uplinkIntYY ) ;

if( NULL == uplinkServer )
	{
	elog	<< "xServer::MSG_S> Unable to find uplink server\n" ;
	return -1 ;
	}

const string serverName( params[ 1 ] ) ;
// Don't care about hop count
time_t startTime = static_cast< time_t >( atoi( params[ 3 ] ) ) ;
time_t connectTime = static_cast< time_t >( atoi( params[ 4 ] ) ) ;
// Don't care about version

int serverIntYY = 0 ;
if( 5 == strlen( params[ 6 ] ) )
	{
	// n2k
	serverIntYY = base64toint( params[ 6 ], 2 ) ;
	}
else
	{
	// yxx
	serverIntYY = base64toint( params[ 6 ], 1 ) ;
	}

// Does the new server's numeric already exist?
if( NULL != Network->findServer( serverIntYY ) )
	{
	elog	<< "xServer::MSG_S> Server numeric collision, numeric: "
		<< params[ 6 ] << ", old name: "
		<< Network->findServer( serverIntYY )->getName()
		<< ", new name: " << serverName << endl ;
	delete Network->removeServer( serverIntYY ) ;
	}

// Dun really care about the server description

iServer* newServer = 0 ;
try
	{
	newServer = new iServer( uplinkIntYY,
		params[ 6 ], // yxx
		serverName,
		connectTime,
		startTime,
		atoi( params[ 5 ] + 1 ) ) ;
	}
catch( std::bad_alloc )
	{
	elog	<< "xServer::MSG_S> Memory allocation failure\n" ;
	return -1 ;
	}

Network->addServer( newServer ) ;
//elog << "Added server: " << *newServer ;

// TODO: Post message
PostEvent( EVT_NETJOIN,
	static_cast< void* >( newServer ),
	static_cast< void* >( uplinkServer ) ) ;

return 0 ;

}

/**
 * GLine message handler.
 * C GL * +~*@209.9.117.131 180 :Banned (~*@209.9.117.131) until 957235403
 *  (On Mon May 1 22:40:23 2000 GMT from SE5 for 180 seconds: remgline
 *  test.. 	[0])
 */
int xServer::MSG_GL( xParameters& )
{
return 0 ;
}

// Channel topics currently are not tracked.
// kAI T #omniplex :-=[ Washington.DC.US.Krushnet.Org / Luxembourg.LU.EU.KrushNet.Org
// Admin Channel ]=-
int xServer::MSG_T( xParameters& )
{
return 0 ;
}

// Q EB
// Q: Remote server numeric
// EB: End Of Burst
int xServer::MSG_EB( xParameters& params )
{

if( !strcmp( params[ 0 ], Uplink->getCharYY() ) )
	{
	// It's my uplink
	burstEnd = ::time( 0 ) ;

	BurstClients() ;

	// Burst our channels
	BurstChannels() ;

	// We are no longer burst
	bursting = false ;

	// For some silly reason, EB must come before EA
	// *shrug*

	// Send our EB
	Write( "%s EB\n", charYY ) ;

	// Acknowledge their end of burst
	Write( "%s EA\n", charYY ) ;

	clog	<< "*** Completed net burst\n" ;
	}

if( !bursting )
	{
	iServer* theServer = Network->findServer( params[ 0 ] ) ;
	if( NULL == theServer )
		{
		elog	<< "xServer::MSG_EB> Unable to find server: "
			<< params[ 0 ] << endl ;
		return -1 ;
		}

	PostEvent( EVT_BURST_CMPLT, static_cast< void* >( theServer ) ) ;
	}

return 0 ;
}

// Q EA
// Q: Remote server numeric
// EA: End Of Burst Acknowledge
// Our uplink server has acknowledged our EB
int xServer::MSG_EA( xParameters& Param )
{
if( !strcmp( Param[ 0 ], Uplink->getCharYY() ) )
	{
	// My uplink! :)
	// Reset EOB just to be sure
	bursting = false ; // ACKNOWLEDGE! :)
	}

if( !bursting )
	{
	iServer* theServer = Network->findServer( Param[ 0 ] ) ;
	if( NULL == theServer )
		{
		elog	<< "xServer::MSG_EA> Unable to find server: "
			<< Param[ 0 ] << endl ;
		return -1 ;
		}

	PostEvent( EVT_BURST_ACK, static_cast< void* >( theServer ) );
	}
return( 0 ) ;

}

// Q G :ripper.ufl.edu
// Q: Remote server numeric
// G: PING
// :ripper.ufl.edu: Ping argument
// Reply with:
// <Our Numeric> Z <Their Numeric> :<arguments>
// Strings will be passed to this method in format:
// Q ripper.ufl.edu
int xServer::MSG_G( xParameters& params )
{
if( params.size() < 2 )
	{
	elog	<< "xServer::MSG_G> Invalid number of parameters\n" ;
	return -1 ;
	}

string s( charYY ) ;
s += " Z " ;
s += params[ 0 ] ;

if( params.size() >= static_cast< xParameters::size_type >( 1 ) )
	{
	s += " :" ;
	s += params[ 1 ] ;
	}

return Write( s ) ;
}

// DeSynch handler?
// 0 DS :HACK: JavaDude MODE #irc.core.com +smtink lamers [957881646]
int xServer::MSG_DS( xParameters& )
{
// TODO
return 0 ;
}

// :Baltimore-R.MD.US.Undernet.Org DESYNCH :HACK: SanDiego.CA.US.Undernet.org MODE #f21
// +o EmMaNuElL <000000000>
int xServer::MSG_Desynch( xParameters& )
{
return 0 ;
}

// Mode change
// OAD M ripper_ :+owg
//
// i M #3dx +o eAA
// J[K M DEMET_33 :+i
int xServer::MSG_M( xParameters& Param )
{

if( Param.size() < 3 )
	{
	elog	<< "xServer::MSG_M> Invalid number of arguments\n" ;
	return -1 ;
	}

// This source stuff really isn't used here, but it's here for
// debugging and validation.
iServer* serverSource = 0 ;
iClient* clientSource = 0 ;

// Note that the order of this if/else if/else is important
if( NULL != strchr( Param[ 0 ], '.' ) )
	{
	// Server, by name
	serverSource = Network->findServerName( Param[ 0 ] ) ;
	}
else if( strlen( Param[ 0 ] ) >= 3 )
	{
	// Client numeric
	clientSource = Network->findClient( Param[ 0 ] ) ;
	}
else
	{
	// 1 or 2 char numeric, server
	serverSource = Network->findServer( Param[ 0 ] ) ;
	}

if( (NULL == clientSource) && (NULL == serverSource) )
	{
	elog	<< "xServer::MSG_M> Unable to find source: "
		<< Param[ 0 ] << endl ;
	// return -1
	}

if( '#' == Param[ 1 ][ 0 ] )
	{
	Channel* theChan = Network->findChannel( Param[ 1 ] ) ;
	if( NULL == theChan )
		{
		elog	<< "xServer::MSG_M> Unable to find channel: "
			<< Param[ 1 ] << endl ;
		return -1 ;
		}

	// This is a bit of a violation of encapsulation.
	// Not too bad though.
	theChan->OnModeChange( clientSource, Param ) ;

	string modes( Param[ 2 ] ) ;

	PostChannelEvent( EVT_MODE, theChan->getName(),
		static_cast< void* >( theChan ),
		static_cast< void* >( clientSource ),
		static_cast< void* >( &modes ) ) ;

	return 0 ;
	}

// Otherwise, it's a user mode change.
// Since users aren't allowed to change modes for anyone other than
// themselves, there is no need to lookup the second user argument
// For some reason, when a user changes his/her/its modes, it still
// specifies the second argument to be nickname instaed of numeric.
iClient* theClient = Network->findNick( Param[ 1 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_M> Unable to find target client: "
		<< Param[ 1 ] << endl ;
	return -1 ;
	}

// Local channels are not propogated across the network.

// It's important that the mode '+' be default
bool plus = true ;

for( const char* modePtr = Param[ 2 ] ; *modePtr ; ++modePtr )
	{
	switch( *modePtr )
		{
		case '+':
			plus = true ;
			break;
		case '-':
			plus = false ;
			break;
		case 'i':
			if( plus )
				theClient->addMode( iClient::MODE_INVISIBLE ) ;
			else
				theClient->removeMode( iClient::MODE_INVISIBLE ) ;
			break ;
		case 'k':
			if( plus )
				theClient->addMode( iClient::MODE_SERVICES ) ;
			else
				theClient->removeMode( iClient::MODE_SERVICES ) ;
			break ;
		case 'd':
			if( plus )
				theClient->addMode( iClient::MODE_DEAF ) ;
			else
				theClient->removeMode( iClient::MODE_DEAF ) ;
			break ;
		case 'w':
			if( plus )
				theClient->addMode( iClient::MODE_WALLOPS ) ;
			else
				theClient->removeMode( iClient::MODE_WALLOPS ) ;
			break ;
		case 'o':
			if( plus )
				{
				theClient->addMode( iClient::MODE_OPER ) ;
				PostEvent( EVT_OPER,
					static_cast< void* >( theClient ) ) ;
				}
			else
				{
				theClient->removeMode( iClient::MODE_OPER ) ;
				// TODO: Post message
				}
			break ;
		default:
			break ;
		} // close switch
	} // close for

// No need to post message here, for now, because the only important
// network event is mode +o/-o
return 0 ;
}

// z MODE #channel -l
// server.name MODE #channel -l
// nickname MODE #channel -l
//
int xServer::MSG_Mode( xParameters& Param )
{

if( Param.size() < 3 )
	{
	elog	<< "xServer::MSG_Mode> Invalid number of arguments\n" ;
	return -1 ;
	}

iClient* clientSource = 0 ;
iServer* serverSource = 0 ;

if( strchr( Param[ 0 ], '.' ) != NULL )
	{
	// Server, by server name
	serverSource = Network->findServerName( Param[ 0 ] ) ;
	}
else
	{
	// Client, by nickname
	clientSource = Network->findNick( Param[ 0 ] ) ;
	}

if( (NULL == clientSource) && (NULL == serverSource) )
	{
	elog	<< "xServer::MSG_Mode> Unable to find source: "
		<< Param[ 0 ] << endl ;
	return -1 ;
	}

if( '#' == Param[ 1 ][ 0 ] )
	{
	Channel* theChan = Network->findChannel( Param[ 1 ] ) ;
	if( NULL == theChan )
		{
		elog	<< "xServer::MSG_Mode> Unable to find channel: "
			<< Param[ 1 ] << endl ;
		return -1 ;
		}

	// This is a bit of a violation of encapsulation.
	// Not too bad though.
	theChan->OnModeChange( clientSource, Param ) ;

	string modes( Param[ 2 ] ) ;

	PostChannelEvent( EVT_MODE, theChan->getName(),
		static_cast< void* >( theChan ),
		static_cast< void* >( clientSource ),
		static_cast< void* >( &modes ) ) ;

	return 0 ;
	}

// Otherwise, it's a user mode change.
// Since users aren't allowed to change modes for anyone other than
// themselves, there is no need to lookup the second user argument
iClient* theClient = clientSource ;

// Local channels are not propogated across the network.

// It's important that the mode '+' be default
bool plus = true ;

for( const char* modePtr = Param[ 2 ] ; *modePtr ; ++modePtr )
	{
	switch( *modePtr )
		{
		case '+':
			plus = true ;
			break;
		case '-':
			plus = false ;
			break;
		case 'i':
			if( plus )
				theClient->addMode( iClient::MODE_INVISIBLE ) ;
			else
				theClient->removeMode( iClient::MODE_INVISIBLE ) ;
			break ;
		case 'k':
			if( plus )
				theClient->addMode( iClient::MODE_SERVICES ) ;
			else
				theClient->removeMode( iClient::MODE_SERVICES ) ;
			break ;
		case 'd':
			if( plus )
				theClient->addMode( iClient::MODE_DEAF ) ;
			else
				theClient->removeMode( iClient::MODE_DEAF ) ;
			break ;
		case 'w':
			if( plus )
				theClient->addMode( iClient::MODE_WALLOPS ) ;
			else
				theClient->removeMode( iClient::MODE_WALLOPS ) ;
			break ;
		case 'o':
			if( plus )
				{
				theClient->addMode( iClient::MODE_OPER ) ;
				PostEvent( EVT_OPER,
					static_cast< void* >( theClient ) ) ;
				}
			else
				{
				theClient->removeMode( iClient::MODE_OPER ) ;
				// TODO: Post message
				}
			break ;
		default:
			break ;
		} // close switch
	} // close for

// No need to post message here, for now, because the only important
// network event is mode +o/-o
return 0 ;
}

static const char* militime( const char* sec, const char* msec )
{
struct timeval tv;
static char buf[128];

gettimeofday( &tv, NULL ) ;

if( sec && msec )
	{
	sprintf( buf, "%d", (int)((tv.tv_sec - atoi(sec)) * 1000 +
		(tv.tv_usec - atoi(msec)) / 1000) ) ;
	}
else
	{
	sprintf( buf, "%d %d", static_cast< int>( tv.tv_sec ),
		static_cast< int >( tv.tv_usec ) ) ;
	}
return buf ;
}

int xServer::MSG_AD( xParameters& )
{
return 0 ;
}

// Remote Ping message
int xServer::MSG_RemPing( xParameters& Param )
{
if( Param[ 1 ][ 0 ] == charYY[ 0 ]
	&& Param[ 1 ][ 1 ] == charYY[ 1 ] && !bursting )
	{
	// It's me
	Write( ":%s RPONG %s %s %s :%s\n",
		ServerName.c_str(),
		Param[ 2 ],
		ServerName.c_str(),
		militime( Param[ 3 ], Param[ 4 ] ),
		Param[ 5 ] ) ;
	}
return 0 ;
}

// Ping message received
// TODO: Only allow opers to receive PONGs
int xServer::MSG_Ping( xParameters& Param )
{
if( Param.size() < 1 )
	{
	elog	<< "xServer::MSG_Ping> Invalid number of arguments\n" ;
	return -1 ;
	}

Write( ":%s PONG :%s\n",
	ServerName.c_str(),
	Param[ 0 ] ) ;
return 0 ;
}

// Depracated.
void AddUser( const unsigned int& Uplink,
	const string& YXX,
	const string& Nick,
	const string& UserID,
	const string& HostBase64,
	const string& InsecureHost,
	const string& Mode, 
	const time_t& ConnectionTime,
	bool IncrementClients = true )
{
// See if the client already exists
iClient* Target = Network->findClient( YXX ) ;

if( Target != NULL )
	{
	// Yup, shoot first, ask questions later
	elog	<< "AddUser> Numeric collision for " << Target->getIntYY()
		<< ":" << Target->getIntXXX() << ":" << Target->getNickName()
		<< endl ;

	// Force remove
	// Don't forget to delete the allocated memory
	delete Network->removeClient( Target->getIntYY(), Target->getIntXXX() ) ;
	}

try
	{
	Target = new iClient( Uplink,
		YXX,
		Nick,
		UserID,
		HostBase64,
		InsecureHost,
		Mode,
		ConnectionTime ) ;
	}
catch( std::bad_alloc )
	{
	// TODO
	elog	<< "AddUser> Memory allocation failure\n" ;
	return ;
	}

//elog << "Adding client: " << *Target ;
if( !Network->addClient( Target ) )
	{
	delete Target ;
	}

}

// Depracated method.
void AddServer( const unsigned int& Uplink,
	const string& YXX,
	const string& ServerName,
	const time_t& ConnectionTime,
	const time_t& StartTime,
	const int& Version )
{

iServer* newServer = NULL ;
try
	{
	newServer = new iServer(
		Uplink,
		YXX,
		ServerName,
		ConnectionTime,
		StartTime,
		Version ) ;
	}
catch( std::bad_alloc )
	{
	elog	<< "AddServer: Memory allocation failure\n" ;
	return ;
	}


// Update this server's info in the network table
if( !Network->addServer( newServer ) )
	{
	delete newServer ;
	elog	<< "AddServer> Unable to add to network table: "
		<< *newServer << endl ;
	}

//elog << "Added server: " << *newServer ;

}

void xServer::dumpStats()
{
clog	<< "Number of channels: " << Network->channelList_size() << endl ;
clog	<< "Number of servers: " << Network->serverList_size() << endl ;
clog	<< "Number of clients: " << Network->clientList_size() << endl ;
clog	<< "Burst duration: " << (burstEnd - burstStart) << " seconds\n" ;
}

xServer::timerID xServer::RegisterTimer( const time_t& absTime,
	xClient* theClient,
	void* data )
{
#ifndef NDEBUG
  assert( theClient != 0 ) ;
#endif

if( absTime >= ::time( 0 ) )
	{
	return 0 ;
	}

// TODO: Make sure this doesn't overflow
timerID ID = lastTimerID++ ;

timerInfo* ti = 0 ;
try
	{
	ti = new timerInfo( ID, absTime, theClient, data ) ;
	}
catch( std::bad_alloc )
	{
	elog	<< "xServer::RegisterTimer> Memory allocation error\n" ;
	return 0 ;
	}

timerQueue.push( timerQueueType::value_type( absTime, ti ) ) ;

return ID ;
}

bool xServer::UnRegisterTimer( xServer::timerID ID )
{

if( timerQueue.empty() )
	{
	return true ;
	}

stack< timerQueueType::value_type > localStack ;
while( !timerQueue.empty() && (timerQueue.top().second->ID != ID) )
	{
	localStack.push( timerQueue.top() ) ;
	timerQueue.pop() ;
	}

bool foundTimer = false ;

// timerQueue is now empty, or its top element has the timer
// we are interested in.
if( !timerQueue.empty() )
	{
	// Find was successful
	foundTimer = true ;

	delete timerQueue.top().second ;
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

if( timerQueue.empty() || bursting )
	{
	return 0 ;
	}

unsigned int numTimers = 0 ;
time_t now = ::time( 0 ) ;

while( timerQueue.top().second->absTime >= now )
	{
	timerInfo* info = timerQueue.top().second ;
	timerQueue.pop() ;

	info->theClient->OnTimer( info->ID, info->data ) ;

	delete info ;
	++numTimers ;
	}

return numTimers ;
}

} // namespace gnuworld
