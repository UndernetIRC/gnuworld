/**
 * server.cc
 * This is the implementation file for the xServer class.
 * This class is the entity which is the GNUWorld server
 * proper.  It manages network I/O, parsing and distributing
 * incoming messages, notifying attached clients of
 * system events, on, and on, and on.
 *
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
 * $Id: server.cc,v 1.138 2002/07/10 14:40:48 dan_karrels Exp $
 */

#include	<sys/time.h>
#include	<unistd.h>

#include	<new>
#include	<string>
#include	<list>
#include	<vector>
#include	<algorithm>
#include	<sstream>
#include	<fstream>
#include	<stack>
#include	<iostream>
#include	<utility>

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
#include	"match.h"
#include	"ELog.h"
#include	"Socket.h"
#include	"StringTokenizer.h"
#include	"xparameters.h"
#include	"moduleLoader.h"
#include	"ServerTimerHandlers.h"
#include	"LoadClientTimerHandler.h"
#include	"UnloadClientTimerHandler.h"
#include	"ConnectionManager.h"
#include	"Connection.h"

const char server_h_rcsId[] = __SERVER_H ;
const char server_cc_rcsId[] = "$Id: server.cc,v 1.138 2002/07/10 14:40:48 dan_karrels Exp $" ;
const char config_h_rcsId[] = __CONFIG_H ;
const char misc_h_rcsId[] = __MISC_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char ip_h_rcsId[] = __IP_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char iServer_h_rcsId[] = __ISERVER_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char EConfig_h_rcsId[] = __ECONFIG_H ;
const char Gline_h_rcsId[] = __GLINE_H ;
const char match_h_rcsId[] = __MATCH_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char Socket_h_rcsId[] = __SOCKET_H ;
const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;
const char moduleLoader_h_rcsId[] = __MODULELOADER_H ;
const char ServerTimerHandler_h_rcsId[] = __SERVERTIMERHANDLERS_H ;
const char LoadClientTimerHandler_h_rcsId[] = __LOADCLIENTTIMERHANDLER_H ;
const char UnloadClientTimerHandler_h_rcsId[] = __UNLOADCLIENTTIMERHANDLER_H ;
const char ConnectionManager_h_rcsId[] = __CONNECTIONMANAGER_H ;
const char Connection_h_rcsId[] = __CONNECTION_H ;

namespace gnuworld
{

using std::pair ;
using std::string ;
using std::vector ;
using std::list ;
using std::endl ;
using std::ends ;
using std::stringstream ;
using std::stack ;
using std::unary_function ;
using std::clog ;
using std::cout ;
using std::min ;

/// The object containing the network data structures
xNetwork*	Network = 0 ;

// Some static xServer variables for tracking signals
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
	elog	<< "Error reading config file: "
		<< configFileName
		<< endl ;
	::exit( -1 ) ;
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

iServer* me = new (std::nothrow) iServer(
	0,
	getCharYYXXX(),
	ServerName,
	::time( 0 ) ) ;
assert( me != 0 ) ;

if( !Network->addServer( me ) )
	{
	elog	<< "xServer::initializeSystem> Failed to add "
		<< "(me) to the system tables"
		<< endl ;
	::exit( -1 ) ;
	}

if( !loadCommandHandlers() )
	{
	elog	<< "xServer::initializeSystem> Failed to load "
		<< "command handlers"
		<< endl ;
	::exit( -1 ) ;
	}

if( !loadClients( configFileName ) )
	{
	elog	<< "xServer> Failed in loading one or more modules"
		<< endl ;
	::exit( -1 ) ;
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
//delete commandMap ;

// Deallocate all of the Glines
for( glineIterator ptr = gline_begin() ; ptr != gline_end() ;
	++ptr )
	{
	delete *ptr ;
	}
glineList.clear() ;

// Deallocate all loaded modules/close dlm handles.
for( clientModuleListType::iterator ptr = clientModuleList.begin() ;
	ptr != clientModuleList.end() ; ++ptr )
	{
	delete *ptr ;
	}
clientModuleList.clear() ;

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
glineUpdateInterval = pingUpdateInterval = 0 ;

// Initialize the numeric stuff.
::memset( charYY, 0, sizeof( charYY ) ) ;
::memset( charXXX, 0, sizeof( charXXX ) ) ;

Network = new (std::nothrow) xNetwork ;
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
commandMapFileName = conf.Require( "command_map" )->second ;

commandHandlerPrefix = conf.Require( "command_handler_path" )->second ;
if( commandHandlerPrefix[ commandHandlerPrefix.size() - 1 ] != '/' )
	{
	commandHandlerPrefix += '/' ;
	}

glineUpdateInterval = static_cast< time_t >( atoi(
	conf.Require( "glineupdateinterval" )->second.c_str() ) ) ;
pingUpdateInterval = static_cast< time_t >( atoi(
	conf.Require( "pingupdateinterval" )->second.c_str() ) ) ;

return true ;
}

bool xServer::loadCommandHandlers()
{
std::ifstream commandMapFile( commandMapFileName.c_str() ) ;
if( !commandMapFile )
	{
	elog	<< "xServer::loadCommandHandlers> Unable to open "
		<< "command map file: "
		<< commandMapFileName
		<< endl ;
	return false ;
	}

string line ;
size_t lineNumber = 0 ;
bool returnVal = true ;

while( std::getline( commandMapFile, line ) )
	{
	++lineNumber ;

	if( line.empty() || '#' == line[ 0 ] )
		{
		continue ;
		}

	StringTokenizer st( line ) ;
	if( st.size() != 2 )
		{
		elog	<< "xServer::loadCommandHandlers> "
			<< commandMapFileName
			<< ":"
			<< lineNumber
			<< "> Invalid syntax, 2 tokens expected, "
			<< st.size()
			<< " tokens found"
			<< endl ;
		returnVal = false ;
		break ;
		}

	// st[ 0 ] is the filename of the module
	// st[ 1 ] is the command key to which the handler will
	//  be registered

	// Let's make sure that the filename is correct
	string fileName( st[ 0 ] ) ;

	// We need the entire path to the command handler in the
	// fileName.
	if( string::npos == fileName.find( commandHandlerPrefix ) )
		{
		// Need to put the command handler path prefix in
		// the filename
		// commandHandlerPrefix has a trailing '/'
		fileName = commandHandlerPrefix + fileName ;
		}

	// All module names end with ".la" for libtool libraries
	if( string::npos == fileName.find( ".la" ) )
		{
		// Need to append ".la" to fileName
		fileName += ".la" ;
		}

	if( !loadCommandHandler( fileName, st[ 1 ] ) )
		{
		elog	<< "xSerer::loadCommandHandlers> Failed to load "
			<< "handler for "
			<< st[ 1 ]
			<< endl ;
		returnVal = false ;
		break ;
		}

//	elog	<< "xServer::loadCommandHandlers> Loaded handler for "
//		<< st[ 1 ]
//		<< endl ;

	} // while()

elog	<< "xServer> Loaded "
	<< commandMap.size()
	<< " command handlers"
	<< endl ;

commandMapFile.close() ;
return returnVal ;
}

bool xServer::loadCommandHandler( const string& fileName,
	const string& commandKey )
{
// Let's first check to see if the module is already open
// It is possible that a single module handler may be
// registered to handle multiple commands (NOOP for example)
commandModuleType* ml = lookupCommandModule( fileName ) ;
if( NULL == ml )
	{
	ml = new (std::nothrow) commandModuleType( fileName ) ;
	assert( ml != 0 ) ;
	}

ServerCommandHandler* sch = ml->loadObject( this ) ;
if( NULL == sch )
	{
	elog	<< "xServer::loadCommandHandler> Error loading "
		<< "module file "
		<< fileName
		<< endl ;

	delete( ml ) ; ml = 0 ;

	return false ;
	}

// Successfully loaded a module
// Put it in the list of modules
commandModuleList.push_back( ml ) ;

// Add the command handler to the handler map
if( !commandMap.insert( commandMapType::value_type(
	commandKey, sch ) ).second )
	{
	elog	<< "xServer::loadCommandHandler> Unable to add "
		<< "handler for message "
		<< commandKey
		<< " to commandMap"
		<< endl ;

	delete ml ; ml = 0 ;
	delete sch ; sch = 0 ;

	return false ;
	}

return true ;
}

xServer::commandModuleType* xServer::lookupCommandModule(
	const string& moduleName ) const
{
for( commandModuleListType::const_iterator ptr =
	commandModuleList.begin() ;
	ptr != commandModuleList.end() ;
	++ptr )
	{
	if( !strcasecmp( (*ptr)->getModuleName(), moduleName ) )
		{
		// Found the module
		return *ptr ;
		}
	}
return 0 ;
}

bool xServer::loadClients( const string& fileName )
{
// Load the config file
EConfig conf( fileName ) ;

/*
 * Load and attach any modules specified in the config.
 */
EConfig::const_iterator ptr = conf.Find( "module" ) ;
for( ; ptr != conf.end() && ptr->first == "module" ; ++ptr )
	{
	StringTokenizer modInfo(ptr->second) ;

	if( 2 != modInfo.size() )
		{
		elog	<< "xServer::loadClients> modules require two "
			<< "arguments, modulename followed by config "
			<< "file name"
			<< endl ;

		return false ;
		}

	elog	<< "xServer> Found module: "
		<< modInfo[0]
		<< " (Config: "
		<< modInfo[1]
		<< ")"
		<< endl;

	// The AttachClient method will load the client from the
	// file.
	if( !AttachClient( modInfo[ 0 ], modInfo[ 1 ] ) )
		{
		// No need for error output here because AttachClient()
		// will do that for us
		elog	<< "xServer::loadClients> Failed to attach client"
			<< endl ;

		return false ;
		}
	}

return true ;
}

void xServer::registerServerTimers()
{
RegisterTimer( ::time( 0 ) + glineUpdateInterval,
	new GlineUpdateTimer( this, glineUpdateInterval ),
	static_cast< void* >( this ) ) ;
RegisterTimer( ::time( 0 ) + pingUpdateInterval,
	new PINGTimer( this, pingUpdateInterval ),
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
int xServer::Connect( const string& Address, unsigned short int Port )
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
	elog	<< "xServer::Connect> Failed to get receive buffer size"
		<< endl ;
	return -1 ;
	}

// Obtain the size of the TCP output window.
// The server will never attempt to write more bytes than
// this to the socket connection.
outputWriteSize = theSock->sendBufSize() ;
if( static_cast< int >( outputWriteSize ) < 0 )
	{
	elog	<< "xServer::Connect> Failed to get output buffer size"
		<< endl ;
	return -1 ;
	}

inputCharBuffer = new (std::nothrow) char[ inputReadSize + 1 ] ;
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

// TODO: Unload clients
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
	elog	<< "xServer::Process> NULL == Sender... *shrug*"
		<< endl ;
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
// elog << "isNumeric: " << isNumeric << endl ;

// Lookup the handler for this command
commandMapType::iterator pairPtr = commandMap.find( Command ) ;
if( pairPtr != commandMap.end() )
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
	if( !pairPtr->second->Execute( Param ) )
		{
		elog	<< "xServer::Process> Handler failed for message: "
			<< Param
			<< endl ;
		}

	}
else
	{
	elog	<< "xServer::Process> Unable to find handler for: "
		<< Command
		<< endl ;
	}
}

/**
 * Squit another server as a server.
 * 0 SQ server.name.com timestamp :reason
 */
bool xServer::SquitServer( const string& serverName, const string& reason )
{

// Is it our server?
if( !strcasecmp( serverName, this->ServerName ) )
	{
	// I don't see that happening
	elog	<< "xServer::SquitServer> Attempt to squit myself!"
		<< endl ;
	return false ;
	}
elog	<< "xServer::SquitServer> Searching for server " << serverName
	<< "\n" ;

// All juped servers are also put into the Network tables...
// This call to findServerName() will find a juped server.
iServer* theServer = Network->removeServerName( serverName ) ;
if( NULL == theServer )
	{
	// The server doesn't exist.
	elog	<< "xServer::SquitServer> Unable to find server: "
		<< serverName
		<< endl ;
	return false ;
	}


for( jupedServerListType::iterator ptr = jupedServers.begin() ;
	ptr != jupedServers.end() ; ++ptr )
	{
	if( *ptr== theServer->getIntYY() )
		{
		// Found the server in the list of jupes
		//elog	<< "xServer::SquitServer> Found " << serverName
		//	<< " in list of juped servers\n" ;
		jupedServers.erase( ptr ) ;
		break ;
		}
	}

// Don't really care if we found a server in the jupe list or not.

// Prepare the output buffer that will squit the server.
stringstream s ;
s	<< charYY
	<< " SQ "
	<< serverName
	<< ' '
	<< theServer->getStartTime()
	<< " :"
	<< reason
	<< ends ;

// Notify the rest of the network of the SQUIT.
Write( s ) ;

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
assert( fakeServer != NULL ) ;

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

// Burst the new server's info./
// IRCu checks for "JUPE " as being the beginning of the
// reason as a jupe server.  This was because before servers
// couldn't link without [ip] being added to their realname
// field unless they were juped by uworld.  Now anyone can
// link with that name, oh well.
Write( "%s S %s %d %d %d J%02d %s 0 :JUPE Reason: %s\n",
		getCharYY(),
		fakeServer->getName().c_str(),
		2,
		0,
		fakeServer->getConnectTime()-24*3600*365,
		10, // version
		charYYXXX.c_str(),
		description.c_str() ) ;

// Write burst acknowledgements.
Write( "%s EB\n", fakeServer->getCharYY() ) ;
Write( "%s EA\n", fakeServer->getCharYY() ) ;

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
assert( theClient != NULL ) ;

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
assert( theClient != NULL ) ;

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
assert( theClient != NULL ) ;

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
assert( theClient != NULL ) ;

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
	void* Data3, void* Data4,
	const xClient* ourClient )
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
	// if he didnt cause the event to trigger
	if((*ptr) != ourClient)
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

assert( theChan != 0 ) ;

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

// srcClient may be NULL, when the source is a server
void xServer::PostChannelKick( Channel* theChan,
	iClient* srcClient,
	iClient* destClient,
	const string& kickMessage,
	bool authoritative )
{
// Public method, verify arguments
assert( theChan != 0 ) ;
// Source can be a server
//assert( srcClient != 0 ) ;

assert( destClient != 0 ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnNetworkKick( theChan,
			srcClient,
			destClient,
			kickMessage,
			authoritative ) ;
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
	(*ptr)->OnNetworkKick( theChan,
		srcClient,
		destClient,
		kickMessage,
		authoritative ) ;
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

// Write at most one half the total output buffer size
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
 * If this method fails, the xClient pointer passed to it is
 * returned to its state when the method was called.
 */
bool xServer::AttachClient( xClient* Client, bool doBurst )
{

// Make sure the pointer is valid.
assert( NULL != Client ) ;

// addClient() will allocate a new YYXXX and
// update Client.
if( !Network->addClient( Client ) )
	{
	elog	<< "xServer::AttachClient> Failed to update network tables"
		<< endl ;
	return false ;
	}

// Let the client know it has been added to
// the server and its tables.
Client->ImplementServer( this ) ;

// TODO: Remove any existing iClient from the xClient

// Create a new iClient representation for this xClient
iClient* theIClient = new (std::nothrow) iClient(
	getIntYY(),
	Client->getCharYYXXX(),
	Client->getNickName(),
	Client->getUserName(),
	"AAAAAA",
	Client->getHostName(),
	Client->getHostName(),
	Client->getModes(),
	"",
	Client->getDescription(),
	::time( 0 ) ) ;
assert( theIClient != 0 ) ;

// Notify the xClient of its iClient instance
Client->setInstance( theIClient ) ;

// Add the iClient to the network tables
if( !Network->addClient( theIClient ) )
	{
	// Failed to add the iClient to the network tables
	elog	<< "xServer::AttachClient> Unable to add theIClient "
		<< "to the Network table"
		<< endl ;

	// We have already reserved a numeric for this client,
	// go ahead and remove it
	Network->removeLocalClient( Client ) ;

	// Do some cleanup
	delete theIClient ; theIClient = 0 ;
	Client->resetInstance() ;

	// Failed to complete the procedure
	return false ;
	}

if( doBurst )
	{
	BurstClient( Client ) ;
	Client->BurstChannels() ;
	}

// Success
return true ;
}

bool xServer::AttachClient( const string& moduleName,
	const string& configFileName,
	bool doBurst )
{

// Create a moduleLoader instance, based on the given moduleName
moduleLoader< xClient* >* ml =
	new (std::nothrow) moduleLoader< xClient* >( moduleName ) ;
assert( ml != 0 ) ;

// Attempt to instantiate an xClient instance from the module
xClient* clientPtr = ml->loadObject( configFileName );

// Check if the object was loaded successfully
if( NULL == clientPtr )
	{
	// Failed to load the object
	elog	<< "xServer::AttachClient> Failed to instantiate module: "
		<< moduleName
		<< endl ;

	// Deallocate the module, this will also close the module file
	delete ml ; ml = 0 ;

	// Return failure
	return false ;
	}

// Attempt to attach the client to the server
if( !AttachClient( clientPtr, doBurst ) )
	{
	// Failed to attach the client
	elog	<< "xServer::AttachClient> Failed to attach new xClient: "
		<< moduleName
		<< endl ;

	// Deallocate the client and its encapsulating module
	delete clientPtr ; clientPtr = 0 ;
	delete ml ; ml = 0 ;

	// Return failure
	return false ;
	}

// The client module is successfully loaded from the file, and
// has been successfully attached to the server

// Add moduleLoader to the clientModuleList
clientModuleList.push_back( ml );

// success
return true ;
}

/**
 * Attach an iClient to a juped server.
 *
 * AQ N ripper_ 1 952038834 ~dan 127.0.0.1 +owg B]AAAB AQAAA :Dan Karrels
 */
bool xServer::AttachClient( iClient* fakeClient )
{
assert( fakeClient != NULL ) ;

// Need to send info to the network about the new client.
iServer* fakeServer = Network->findServer( fakeClient->getIntYY() ) ;
if( NULL == fakeServer )
	{
	elog	<< "xServer::AttachClient> Unable to find fake server: "
		<< fakeClient->getIntYY()
		<< endl ;
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
bool xServer::DetachClient( xClient* Client, const string& reason )
{
if( NULL == Client )
	{
	return false ;
	}

// Notify the client that it is being detached.
Client->Exit( reason ) ;

// removeClient() does all of the internal updates
removeClient( Client ) ;

return true ;
}

/**
 * Detach an xClient by moduleName
 */
bool xServer::DetachClient( const string& moduleName,
	const string& reason )
{
for( clientModuleListType::const_iterator ptr = clientModuleList.begin() ;
	ptr != clientModuleList.end() ; ++ptr )
	{
	if( !strcasecmp( (*ptr)->getModuleName(), moduleName ) )
		{
		// Found one
		return DetachClient( (*ptr)->getObject(), reason ) ;
		}
	}
return false ;
}

void xServer::LoadClient( const string& moduleName,
	const string& configFileName )
{
elog	<< "xServer::LoadClient("
	<< moduleName
	<< ", "
	<< configFileName
	<< ")"
	<< endl ;

// First, unload the client.
// This will queue the request.
//UnloadClient( moduleName ) ;

// Next, queue the load request
LoadClientTimerHandler* handler = new (std::nothrow)
	LoadClientTimerHandler( this, moduleName, configFileName ) ;
assert( handler != 0 ) ;

RegisterTimer( ::time( 0 ), handler, 0 ) ;
}

void xServer::UnloadClient( const string& moduleName,
	const string& reason )
{
elog	<< "xServer::UnloadClient(const string&)> "
	<< moduleName
	<< endl ;

UnloadClientTimerHandler* handler = new (std::nothrow)
	UnloadClientTimerHandler( this, moduleName, reason ) ;
assert( handler != 0 ) ;

RegisterTimer( ::time( 0 ), handler, 0 ) ;
}

void xServer::UnloadClient( xClient* theClient, const string& reason )
{
elog	<< "xServer::UnloadClient(xClient*)> "
	<< theClient->getNickName()
	<< endl ;

for( clientModuleListType::const_iterator ptr = clientModuleList.begin() ;
	ptr != clientModuleList.end() ; ++ptr )
	{
	if( (*ptr)->getObject() == theClient )
		{
		// Found one
		UnloadClient( (*ptr)->getModuleName(), reason ) ;
		return ;
		}
	}

elog	<< "xServer::UnloadClient(xClient*)> Unable to find client: "
	<< theClient->getNickName()
	<< endl ;
}

// This method is responsible for updating all internal
// tables and deallocating the given xClient
void xServer::removeClient( xClient* theClient )
{
// Precondition: theClient != 0

// Remove this xClient's iClient instance
delete Network->removeClient( theClient->getInstance() ) ;
theClient->resetInstance() ;

// Walk the channelEventMap, and remove the xClient from all
// channel's in which it is registered.
// This is an O(n) operation
for( channelEventMapType::iterator chPtr = channelEventMap.begin(),
	chEndPtr = channelEventMap.end() ;
	chPtr != chEndPtr ;
	++chPtr )
	{
	(*chPtr).second->remove( theClient ) ;
	}

// Remove this xClient from all other events
for( eventListType::iterator evPtr = eventList.begin(),
	evEndPtr = eventList.end() ;
	evPtr != evEndPtr ;
	++evPtr )
	{
	(*evPtr).remove( theClient ) ;
	}

// Remove the client from the network data structures.
// Be sure to do this before closing the client's module,
// because closing the module will invalidate the client
// pointer.
Network->removeLocalClient( theClient ) ;

// Find this client in the module list
for( clientModuleListType::iterator modPtr = clientModuleList.begin() ;
	modPtr != clientModuleList.end() ; ++modPtr )
	{
	if( (*modPtr)->getObject() == theClient )
		{
		// We need to deallocate the client object
		// here before we close the module.
		delete theClient ; theClient = 0 ;

		// Deallocating the module will close
		// the module as well.
		delete *modPtr ;

		// Remove this module from the list of modules
		clientModuleList.erase( modPtr ) ;

		break ;
		}
	}
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
 * Write the contents of a std::stringstream to the uplink connection.
 */
size_t xServer::Write( const stringstream& s )
{
return Write( string( s.str() ) ) ;
}

size_t xServer::WriteDuringBurst( const stringstream& s )
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
vsnprintf( buffer, 4096, format, _list ) ;
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
vsnprintf( buffer, 4096, format, _list ) ;
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

bool xServer::removeGline( const string& userHost, const xClient* remClient )
{

// This method is true if we find the gline in our internal
// structure of glines.
bool foundGline = false ;

// Perform a linear search for the gline
glineIterator ptr = gline_begin() ;
for( ; ptr != gline_end() ; ++ptr )
	{
	// Is this the gline in question?
	if(strcasecmp((*ptr)->getUserHost(),userHost))
		{
		continue;
		}
	// Yup, found it
	foundGline = true ;
	break ;
	}

// Found it, notify the network that we are removing it
stringstream s ;
s	<< charYY
	<< " GL * -"
	<< userHost
	<< ends ;

// Write the data to the network output buffer(s)
Write( s ) ;

// Did we find the gline in the interal gline structure?
if( foundGline )
	{
	// Remove the gline from the internal gline structure
	glineList.erase( ptr ) ;

	// Let all clients know that the gline has been removed
	if(remClient)
		{
		PostEvent( EVT_REMGLINE,
			static_cast< void* >( *ptr ), 0,0,0,remClient ) ;
		}
	else
		{
		PostEvent( EVT_REMGLINE,
			static_cast< void* >( *ptr ) ) ;
		}
	// Deallocate the gline
	delete *ptr ;

	}

// Return success
return foundGline ;
}

// C GL * +~*@209.9.117.131 180 :Banned (~*@209.9.117.131) until 957235403 (On Mon May  1
// 22:40:23 2000 GMT from SE5 for 180 seconds: remgline test.. 	[0])
bool xServer::setGline(
	const string& setBy,
	const string& userHost,
	const string& reason,
	const time_t& duration,
	const xClient* setClient,
	const string& server )
{

// Remove any old matches
removeMatchingGlines( userHost ) ;

Gline* newGline =
	new (std::nothrow) Gline( setBy, userHost, reason, duration ) ;
assert( newGline != 0 ) ;

// Notify the rest of the network
stringstream s ;
s	<< getCharYY() << " GL "
	<< server << " +"
	<< userHost << ' '
	<< duration << " :"
	<< reason << ends ;
Write( s ) ;

glineList.push_back( newGline ) ;
if(setClient)
	{
	PostEvent( EVT_GLINE,
		static_cast< void* >( newGline ), 0,0,0,setClient ) ;
	}
else
	{
	PostEvent( EVT_GLINE,
		static_cast< void* >( newGline ) ) ;
	}

return true ;
}

vector< const Gline* > xServer::matchGline( const string& userHost ) const
{
vector< const Gline* > retMe ;

for( const_glineIterator ptr = gline_begin() ;
	ptr != gline_end() ; ++ptr )
	{
	if( !match( (*ptr)->getUserHost(), userHost ) )
		{
		retMe.push_back( *ptr ) ;
		}
	}

return retMe ;
}

const Gline* xServer::findGline( const string& userHost ) const
{
for( const_glineIterator ptr = gline_begin() ;
	ptr != gline_end() ; ++ptr )
	{
	if( !strcasecmp( (*ptr)->getUserHost(), userHost ) )
		{
		// Found it
		return *ptr ;
		}
	}
return 0 ;
}

void xServer::sendGlinesToNetwork()
{
time_t now = ::time( 0 ) ;

for( const_glineIterator ptr = gline_begin() ;
	ptr != gline_end() ; ++ptr )
	{
	stringstream s ;
	s	<< getCharYY() << " GL * +"
		<< (*ptr)->getUserHost() << ' '
		<< ((*ptr)->getExpiration() - now) << " :"
		<< (*ptr)->getReason() << ends ;

	Write( s ) ;
	}
}

void xServer::removeMatchingGlines( const string& wildHost )
{
for( glineIterator ptr = gline_begin() ; ptr != gline_end() ; )
	{
	// TODO: Does this work with two wildHost's?
	if( !strcasecmp( wildHost, (*ptr)->getUserHost() ) )
		{
		ptr = glineList.erase( ptr ) ;

		PostEvent( EVT_REMGLINE,
			static_cast< void* >( *ptr ) ) ;
		}
	else
		{
		++ptr ;
		}
	}
}

/**
 * Write an xClient channel part to the network, and update
 * network tables.
 */
void xServer::PartChannel( xClient* theClient, const string& chanName,
	const string& reason )
{
assert( theClient != NULL ) ;

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::PartChannel> Unable to find channel: "
		<< chanName
		<< endl ;
	return ;
	}

PartChannel( theClient, theChan, reason ) ;
}

/**
 * Write an xClient channel part to the network, and update
 * network tables.
 */
void xServer::PartChannel( xClient* theClient, Channel* theChan,
	const string& reason )
{
assert( theClient != 0 ) ;
assert( theChan != 0 ) ;

stringstream s ;
s	<< theClient->getCharYYXXX()
	<< " L "
	<< theChan->getName()
	<< " :"
	<< reason
	<< ends ;

Write( s ) ;

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
assert( theClient != NULL ) ;

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::OnPartChannel> Unable to find channel: "
		<< chanName
		<< endl ;
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
assert( theClient != 0 ) ;
assert( theChan != 0 ) ;

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
assert( theClient != NULL ) ;

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::OnPartChannel> Unable to find channel: "
		<< chanName
		<< endl ;
	return ;
	}
OnPartChannel( theClient, theChan ) ;

}

void xServer::OnPartChannel( xClient* theClient, Channel* theChan )
{
assert( theClient != 0 ) ;
assert( theChan != 0 ) ;
}

bool xServer::JoinChannel( xClient* theClient,
	const string& chanName,
	const string& chanModes,
	const time_t& joinTime,
	bool getOps )
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
	stringstream s ;
	s	<< getCharYY()
		<< " B "
		<< chanName << ' '
		<< postJoinTime << ' '
		<< chanModes << ' '
		<< theClient->getCharYYXXX() ;

	if( getOps )
		{
		s	<< ":o" ;
		}

		s	<< ends ;

	Write( s ) ;

	// Instantiate the new channel
	theChan = new (std::nothrow) Channel( chanName, time( 0 ) ) ;
	assert( theChan != 0 ) ;

	// Add it to the network channel table
	if( !Network->addChannel( theChan ) )
		{
		elog	<< "xServer::JoinChannel> addChannel() "
			<< "failed: "
			<< theChan->getName()
			<< endl ;

		// Prevent a memory leak
		delete theChan ; theChan = 0 ;

		// Return failure
		return false ;
		}

	// We have just burst a channel

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
	stringstream s ;
	s	<< theClient->getCharYYXXX()
		<< " C "
		<< chanName
		<< ' '
		<< postJoinTime
		<< ends ;
	Write( s ) ;
	}

	if( !chanModes.empty() )
		{
		stringstream s ;
		s	<< theClient->getCharYYXXX()
			<< " M "
			<< chanName << ' '
			<< chanModes
			<< ends ;
		Write( s ) ;
		}

	// Instantiate the new channel
	theChan = new (std::nothrow) Channel( chanName, time( 0 ) ) ;
	assert( theChan != 0 ) ;

	// Add it to the network channel table
	if( !Network->addChannel( theChan ) )
		{
		elog	<< "xServer::JoinChannel> addChannel() "
			<< "failed: "
			<< theChan->getName()
			<< endl ;

		// Prevent a memory leak
		delete theChan ; theChan = 0 ;

		// Return failure
		return false ;
		}

	// When we create a channel, the client automatically gets
	// ops
	getOps = true ;

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

	if( postJoinTime > theChan->getCreationTime() )
		{
		// We are bursting into a channel that has an
		// older timestamp than the one we where supplied with.
		// We use the existing timestamp to remain in sync
		// (And get op'd if needs be).
		postJoinTime = theChan->getCreationTime();
		}

	// TODO: If the timestamp we are bursting is less than the
	// existing one, we need to set the our Network channel state to
	// match that supplied in this line. (Because we are authoritive
	// in this channel, any existing modes will be removed by ircu).
	stringstream s ;
	s	<< getCharYY()
		<< " B "
		<< chanName << ' '
		<< postJoinTime << ' '
		<< chanModes << ' '
		<< theClient->getCharYYXXX() ;

	if( getOps )
		{
		s	<< ":o" ;
		}

		s	<< ends ;

	Write( s ) ;
	}
else
	{
	// After bursting, and the channel exists
		{
		stringstream s2 ;
		s2	<< theClient->getCharYYXXX()
			<< " J "
			<< chanName
			<< ends ;

		Write( s2 ) ;
		}

	if( getOps )
		{
		// Op the bot
		stringstream s ;
		s	<< charYY
			<< " M "
			<< chanName
			<< " +o "
			<< theClient->getCharYYXXX()
			<< ends ;

		Write( s ) ;
		}

	if( !chanModes.empty() )
		{
		// Set the channel modes
		stringstream s ;
		s	<< theClient->getCharYYXXX() << " M "
			<< chanName << ' '
			<< chanModes << ends ;

		Write( s ) ;
		}
	}

if( postJoinTime < theChan->getCreationTime() )
	{
	theChan->setCreationTime( joinTime ) ;
	}

// Is the string not empty, and not only consisting of spaces?
if( !chanModes.empty() &&
	(string::npos != chanModes.find_first_not_of( ' ' )))
	{
	StringTokenizer st( chanModes ) ;
	StringTokenizer::size_type argPos = 1 ;

	bool plus = true ;

	for( string::const_iterator ptr = st[ 0 ].begin() ;
		ptr != st[ 0 ].end() ; ++ptr )
		{
		switch(  *ptr )
			{
			case '+':
				plus = true ;
				break ;
			case '-':
				plus = false ;
				break ;
			case 't':
				if( plus )
					theChan->onModeT( true ) ;
				else
					theChan->onModeT( false ) ;
				break ;
			case 'n':
				if( plus )
					theChan->onModeN( true ) ;
				else
					theChan->onModeN( false ) ;
				break ;
			case 's':
				if( plus )
					theChan->onModeS( true ) ;
				else
					theChan->onModeS( false ) ;
				break ;
			case 'p':
				if( plus )
					theChan->onModeP( true ) ;
				else
					theChan->onModeP( false ) ;
				break ;
			case 'm':
				if( plus )
					theChan->onModeM( true ) ;
				else
					theChan->onModeM( false ) ;
				break ;
			case 'i':
				if( plus )
					theChan->onModeI( true ) ;
				else
					theChan->onModeI( false ) ;
				break ;

			// TODO: Finish with polarity
			// TODO: Add in support for modes b,v,o
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

// Add the channel to the iClient's info
theIClient->addChannel( theChan ) ;

// Create a new ChannelUser instance for the channel's records
ChannelUser* theChanUser = new (std::nothrow) ChannelUser( theIClient ) ;

// Make sure the allocation was successful
assert( theChanUser != 0 ) ;

// Did the xClient request ops in the channel?
if( getOps )
	{
	// Yes, update the ChannelUser's info to reflect its
	// operator state
	theChanUser->setModeO() ;
	}

// Add the ChannelUser to the channel
if( !theChan->addUser( theChanUser ) )
	{
	elog	<< "xServer::JoinChannel> Unable to add xClient ("
		<< theClient->getNickName()
		<< ") to channel "
		<< theChan->getName()
		<< endl ;

	// TODO
	return false ;
	}

return true ;

}

// K N Isomer 2 957217279 ~perry p136-tnt1.ham.ihug.co.nz DLbaCI KAC :*Unknown*
void xServer::BurstClient( xClient* theClient, bool localClient )
{
stringstream s ;
s	<< getCharYY() << " N "
	<< theClient->getNickName() << ' '
	<< (localClient ? '1' : '2') << " 31337 "
	<< theClient->getUserName() << ' '
	<< theClient->getHostName() << ' '
	<< theClient->getModes() << ' '
	<< "AAAAAA" << ' '
	<< theClient->getCharYYXXX() << " :"
	<< theClient->getDescription() << ends ;
Write( s ) ;

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
assert( theHandler != 0 ) ;

// Allow registration of timers which are requesting to be executed
// at times which are <= now.
// This will just allow the timer to run on the next iteration

// Retrieve a unique timerID
timerID ID = getUniqueTimerID() ;

// Allocate a timerInfo structure to represent this timer
timerInfo* ti =
	new (std::nothrow) timerInfo( ID, absTime, theHandler, data ) ;
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

//TODO: figure out why foreach_xClient doesnt work

//Network->foreach_xClient( handleSignal( whichSig ) ) ;


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
	case SIGUSR2:
		retMe = true;
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
void xServer::OnChannelModeT( Channel* theChan, bool polarity,
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
void xServer::OnChannelModeN( Channel* theChan, bool polarity,
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
void xServer::OnChannelModeS( Channel* theChan, bool polarity,
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
void xServer::OnChannelModeP( Channel* theChan, bool polarity,
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
void xServer::OnChannelModeM( Channel* theChan, bool polarity,
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
void xServer::OnChannelModeI( Channel* theChan, bool polarity,
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
void xServer::OnChannelModeL( Channel* theChan, bool polarity,
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
void xServer::OnChannelModeK( Channel* theChan, bool polarity,
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
void xServer::OnChannelModeO( Channel* theChan, ChannelUser* sourceUser,
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
void xServer::OnChannelModeV( Channel* theChan, ChannelUser* sourceUser,
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
void xServer::OnChannelModeB( Channel* theChan, ChannelUser* sourceUser,
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
	OnChannelModeT( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_N ) )
	{
	OnChannelModeN( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_S ) )
	{
	OnChannelModeS( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_P ) )
	{
	OnChannelModeP( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_M ) )
	{
	OnChannelModeM( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_I ) )
	{
	OnChannelModeI( theChan, false, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_L ) )
	{
	OnChannelModeL( theChan, false, 0, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_K ) )
	{
	OnChannelModeK( theChan, false, 0, string() ) ;
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
	OnChannelModeO( theChan, 0, opVector ) ;
	}
if( !voiceVector.empty() )
	{
	OnChannelModeV( theChan, 0, voiceVector ) ;
	}
if( !banVector.empty() )
	{
	OnChannelModeB( theChan, 0, banVector ) ;
	}
}

void xServer::updateGlines()
{
time_t now = ::time( 0 ) ;

glineIterator	ptr = gline_begin(),
		end = gline_end() ;
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

bool xServer::RemoveJupe( const iServer* theServer )
{
for( jupedServerListType::iterator ptr = jupedServers.begin() ;
	ptr != jupedServers.end() ; ++ptr )
	{
	if( *ptr== theServer->getIntYY() )
		{
		// Found the server in the list of jupes
		//elog	<< "xServer::RemoveJupe> Found "
		//	<< serverName
		//	<< " in list of juped servers\n" ;

		jupedServers.erase( ptr ) ;

		return true ;
		}
	}
return false ;
}

int xServer::Wallops( const string& msg )
{
if( msg.empty() )
	{
	return -1 ;
	}

stringstream s ;
s	<< getCharYY()
	<< " WA :"
	<< msg
	<< ends ;

return Write( s ) ;
}

int xServer::Mode( xClient* theClient,
	Channel* theChan,
	const string& modes,
	const string& args )
{
assert( theChan != 0 ) ;

if( NULL == theClient )
	{
	return -10 ;
	}

// Make sure that the modes string is not empty, it's ok for
// the args string to be empty
if( modes.empty() )
	{
	return -1 ;
	}

// Return the iClient instance for the requesting xClient
iClient* theIClient = theClient->getInstance() ;

// Make sure the pointer is not NULL
assert( theIClient != 0 ) ;

// Attempt to find the ChannelUser for the requesting xClient
// on the given channel
ChannelUser* theUser = theChan->findUser( theIClient ) ;

// Is the xClient in the channel?
if( NULL == theIClient )
	{
	// Nope, the xClient is not in the channel...silly xClient
	return -9 ;
	}

// Verify that the modes and args are valid for the given channel,
// this will allow the below loop to work successfully and it
// will not require to do any validation
int retMe = parseModeRequest( theChan, modes, args ) ;
if( retMe < 0 )
	{
	// Something was wrong with the modes and/or args mode
	// change parameters...return the error found from
	// parseModeRequest()
	return retMe ;
	}

// Tokenize the argument list by ' '
StringTokenizer argTokens( args ) ;

// The argument iterator
StringTokenizer::const_iterator argPtr = argTokens.begin() ;

// This variable holds the polarity of the current mode
bool polarity = true ;

// These structures will queue all modes to be sent to the channel
// and all other xClients
opVectorType opVector ;
voiceVectorType voiceVector ;
banVectorType banVector ;

for( string::const_iterator modePtr = modes.begin() ;
	modePtr != modes.end() ; ++modePtr )
	{
	switch( *modePtr )
		{
		case '+':
			polarity = true ;
			break ;
		case '-':
			polarity = false ;
			break ;
		case 't':
			OnChannelModeT( theChan, polarity,
				theUser ) ;
			break ;
		case 'n':
			OnChannelModeN( theChan, polarity,
				theUser ) ;
			break ;
		case 's':
			OnChannelModeS( theChan, polarity,
				theUser ) ;
			break ;
		case 'p':
			OnChannelModeP( theChan, polarity,
				theUser ) ;
			break ;
		case 'i':
			OnChannelModeI( theChan, polarity,
				theUser ) ;
			break ;
		case 'm':
			OnChannelModeM( theChan, polarity,
				theUser ) ;
			break ;
		case 'b':
			banVector.push_back( banVectorType::value_type(
				polarity, *argPtr ) ) ;
			++argPtr ;
			break ;
		case 'v':
			{
			iClient* theClient = Network->findClient(
				*argPtr ) ;
			ChannelUser* theUser = theChan->findUser(
				theClient ) ;

			voiceVector.push_back( voiceVectorType::value_type(
				polarity, theUser ) ) ;
			++argPtr ;
			}
			break ;
		case 'o':
			{
			iClient* theClient = Network->findClient(
				*argPtr ) ;
			ChannelUser* theUser = theChan->findUser(
				theClient ) ;

			opVector.push_back( opVectorType::value_type(
				polarity, theUser ) ) ;
			++argPtr ;
			}
			break ;
		case 'k':
			// Channel mode 'k' always has an argument
			OnChannelModeK( theChan, polarity, theUser,
				*argPtr ) ;
			++argPtr ;
			break ;
		case 'l':
			// Channel mode 'l' only has an argument if
			// it's being added, but not removed
			OnChannelModeL( theChan, polarity, theUser,
				polarity ? atoi( (*argPtr).c_str() )
				: 0 ) ;
			if( polarity )
				{
				++argPtr ;
				}
			break ;
		} // switch()
	} // for()

// Write the modes to the network before updating tables and notifying
// other xClients...this will keep the output buffers synched
stringstream s ;
s	<< getCharYY()
	<< ' '
	<< theChan->getName()
	<< ' '
	<< modes ;

if( !args.empty() )
	{
	s	<< ' '
		<< args ;
	}

s	<< ends ;

retMe = Write( s ) ;

// Update internal tables and notify all xClients of mode change(s)
if( !opVector.empty() )
	{
	OnChannelModeO( theChan, theUser, opVector ) ;
	}
if( !voiceVector.empty() )
	{
	OnChannelModeV( theChan, theUser, voiceVector ) ;
	}
if( !banVector.empty() )
	{
	OnChannelModeB( theChan, theUser, banVector ) ;
	}

return retMe ;

}

int xServer::parseModeRequest( const Channel* theChan,
	const string& modes,
	const string& args ) const
{

// Tokenize the argument list by ' '
StringTokenizer argTokens( args ) ;

// The argument iterator
StringTokenizer::const_iterator argPtr = argTokens.begin() ;

// This variable holds the polarity of the current mode
bool polarity = true ;

for( string::const_iterator modePtr = modes.begin() ;
	modePtr != modes.end() ; ++modePtr )
	{
	switch( *modePtr )
		{
		case '+':
			polarity = true ;
			break ;
		case '-':
			polarity = false ;
			break ;
		case 'b':
			{
			if( argPtr == argTokens.end() )
				{
				// Insufficient arguments
				return -2 ;
				}
			if( !banSyntax( *argPtr ) )
				{
				// Bad ban syntax
				return -3 ;
				}
			if( !polarity )
				{
				// Removing a ban, make sure it
				// exists
				if( !theChan->findBan( *argPtr ) )
					{
					// Ban mask not found
					return -4 ;
					}
				}
			// All checks ok, increment argument iterator
			++argPtr ;

			} // case 'b'
			break ;
		case 'o':
		case 'v':
			{
			if( argPtr == argTokens.end() )
				{
				// Insufficient arguments
				return -2 ;
				}
			iClient* theClient = Network->findClient(
				*argPtr ) ;
			if( NULL == theClient )
				{
				// Client not found
				return -4 ;
				}

			if( NULL == theChan->findUser( theClient ) )
				{
				// ChannelUser not found
				return -4 ;
				}

			// All is well
			++argPtr ;

			} // case 'o'/case 'v'
			break ;
		case 'l':
			// Only has an argument if it's being added
			if( polarity )
				{
				if( argPtr == argTokens.end() )
					{
					// Insufficient arguments
					return -2 ;
					}
				++argPtr ;
				}
			break ;
		case 'k':
			// Mode 'k' always has an argument
			if( argPtr == argTokens.end() )
				{
				// Insufficient arguments
				return -2 ;
				}
			++argPtr ;

			break ;
		case 't':
		case 'n':
		case 'i':
		case 'm':
		case 'p':
		case 's':
			// No big deal, let the network deal with any
			// problems
			break ;
		default:
			// Unknown mode
			return -5 ;
			break ;

		} // switch()
	} // for()

// All is well
return 0 ;

}

// Make sure the banMask is of the form nick!user@host
bool xServer::banSyntax( const string& theMask ) const
{
string::size_type exPos = theMask.find( '!' ) ;
string::size_type atPos = theMask.find( '@' ) ;

if( (string::npos == exPos) || (string::npos == atPos) ||
	(exPos > atPos) )
	{
	return false ;
	}

return true ;
}

bool xServer::isJuped( const iServer* theServer ) const
{
for( jupedServerListType::const_iterator ptr = jupedServers.begin() ;
	ptr != jupedServers.end() ; ++ptr )
	{
	if( *ptr == theServer->getIntYY() )
		{
		return true;
		}
	}

return false;
}

} // namespace gnuworld
