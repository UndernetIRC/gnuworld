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
 * $Id: server.cc,v 1.171 2003/07/30 22:06:21 dan_karrels Exp $
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
#include	"StringTokenizer.h"
#include	"xparameters.h"
#include	"moduleLoader.h"
#include	"ServerTimerHandlers.h"
#include	"LoadClientTimerHandler.h"
#include	"UnloadClientTimerHandler.h"
#include	"ConnectionManager.h"
#include	"ConnectionHandler.h"
#include	"Connection.h"

RCSTAG( "$Id: server.cc,v 1.171 2003/07/30 22:06:21 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::pair ;
using std::string ;
using std::vector ;
using std::list ;
using std::endl ;
using std::stringstream ;
using std::stack ;
using std::unary_function ;
using std::clog ;
using std::cout ;
using std::min ;

/// The object containing the network data structures
xNetwork*	Network = 0 ;

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
	delete ptr->second ;
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

} // ~xServer()

void xServer::initializeVariables()
{

// Initialize more variables
keepRunning = true ;
bursting = false ;
useHoldBuffer = false ;
StartTime = ::time( NULL ) ;

serverConnection = 0 ;
burstStart = burstEnd = 0 ;
Uplink = NULL ;
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

iClient::setHiddenHostSuffix(
	conf.Require( "hidden_host_suffix" )->second ) ;

libPrefix = conf.Require( "libdir" )->second ;
if( libPrefix[ libPrefix.size() - 1 ] != '/' )
	{
	libPrefix += '/' ;
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

	// module_file_name module_loader_symbol command_key
	StringTokenizer st( line ) ;
	if( st.size() != 3 )
		{
		elog	<< "xServer::loadCommandHandlers> "
			<< commandMapFileName
			<< ":"
			<< lineNumber
			<< "> Invalid syntax, 3 tokens expected, "
			<< st.size()
			<< " tokens found"
			<< endl ;
		returnVal = false ;
		break ;
		}

	// st[ 0 ] is the module file name
	// st[ 1 ] is the symbol name to lookup, minus the preceeding
	// _gnuwinit_
	// st[ 2 ] is the command key to which the handler will
	//  be registered

	// Let's make sure that the filename is correct
	string fileName( st[ 0 ] ) ;

	// We need the entire path to the command handler in the
	// fileName.
	if( string::npos == fileName.find( libPrefix ) )
		{
		// Need to put the command handler path prefix in
		// the filename
		// libPrefix has a trailing '/'
		fileName = libPrefix + fileName ;
		}

	// All module names end with ".la" for libtool libraries
	if( string::npos == fileName.find( ".la" ) )
		{
		// Need to append ".la" to fileName
		fileName += ".la" ;
		}

	if( !loadCommandHandler( fileName, st[ 1 ], st[ 2 ] ) )
		{
		elog	<< "xServer::loadCommandHandlers> Failed to load "
			<< "handler for message token "
			<< st[ 2 ]
			<< ", from module file: "
			<< fileName
			<< ", with symbol suffic: "
			<< st[ 1 ]
			<< endl ;
		returnVal = false ;
		break ;
		}

//	elog	<< "xServer::loadCommandHandlers> Loaded handler for "
//		<< st[ 2 ]
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
	const string& symbolName,
	const string& commandKey )
{
// Let's first check to see if the module is already open
// It is possible that a single module handler may be
// registered to handle multiple commands (NOOP for example)
bool foundExistingModule = true ;
commandModuleType* ml = lookupCommandModule( fileName ) ;
if( NULL == ml )
	{
	foundExistingModule = false ;
	ml = new (std::nothrow) commandModuleType( fileName ) ;
	assert( ml != 0 ) ;
	}
else
	{
//	elog	<< "xServer::loadCommandHandler> Found existing module: "
//		<< ml->getModuleName()
//		<< endl ;
	}

string symbolSuffix = string( "_" ) + symbolName ;
//elog	<< "xServer::loadCommandHandler> fileName: "
//	<< fileName
//	<< ", symbolSuffix: "
//	<< symbolSuffix
//	<< ", commandKey: "
//	<< commandKey
//	<< endl ;

ServerCommandHandler* sch = ml->loadObject( this, symbolSuffix ) ;
if( NULL == sch )
	{
	elog	<< "xServer::loadCommandHandler> Failed to load "
		<< "handler for message token "
		<< commandKey
		<< ", from module file: "
		<< fileName
		<< ", with symbol suffic: "
		<< symbolName
		<< endl ;

	delete( ml ) ; ml = 0 ;

	return false ;
	}

// Successfully loaded a module
// Put it in the list of modules
if( !foundExistingModule )
	{
	// No sense in adding the same module multiple times
	commandModuleList.push_back( ml ) ;
	}

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

	elog	<< "xServer::loadClients> Found module: "
		<< modInfo[0]
		<< " (Config: "
		<< modInfo[1]
		<< ")"
		<< endl;

	string fileName = modInfo[ 0 ] ;
	if( '/' != fileName[ 0 ] )
		{
		// Relative path, prepend the libPrefix to the fileName
		// libPrefix is guaranteed to end with '/'
		fileName = libPrefix + modInfo[ 0 ] ;
		}

	// The AttachClient method will load the client from the
	// file.
	if( !AttachClient( fileName, modInfo[ 1 ] ) )
		{
		// No need for error output here because AttachClient()
		// will do that for us
		elog	<< "xServer::loadClients> Failed to attach client: "
			<< fileName
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
keepRunning = false ;

// Can't call removeClients() here because it is likely one of the
// clients that has invoked this call, that would be bad.
// Instead, the code that will halt the system is located in the
// main for() loop, which simply calls doShutdown() (private method).
}

void xServer::OnConnect( Connection* theConn )
{
// Just connected to our uplink
serverConnection = theConn ;

// P10 version information, bogus.
Version = 10 ;

// Initialize the connection time variable to current time.
ConnectionTime = ::time( NULL ) ;

clog	<< "*** Connected!"
	<< endl ;

elog	<< "*** Connected to "
	<< serverConnection->getHostname()
	<< ", port "
	<< serverConnection->getRemotePort()
	<< endl ;

// Login to the uplink.
WriteDuringBurst( "PASS :%s\n", Password.c_str() ) ;

// Send our server information.
WriteDuringBurst( "SERVER %s %d %d %d J%02d %s +s :%s\n",
	        ServerName.c_str(),
		1,
		StartTime,
		ConnectionTime,
		Version,
		(string( getCharYY() ) + "]]]").c_str(),
		ServerDescription.c_str() ) ;
}

void xServer::OnConnectFail( Connection* theConn )
{
elog	<< "xServer::OnConnectFail> Failed to establish connection "
	<< "to "
	<< theConn->getHostname()
	<< ":"
	<< theConn->getRemotePort()
	<< endl ;

keepRunning = false ;
}

/**
 * Handle a disconnect from our uplink.  This method is
 * responsible for deallocating variables mostly.
 */
void xServer::OnDisconnect( Connection* theConn )
{
if( theConn != serverConnection )
	{
	elog	<< "xServer::OnDisconnect> Unknown connection"
		<< endl ;
	return ;
	}

// Disconnected from uplink
// The ConnectionManager will deallocate the memory associated with
// the Connection object
serverConnection = 0 ;

elog	<< "xServer::OnDisconnect> Disconnected :("
	<< endl ;

keepRunning = false ;

// doShutdown() will be called at the bottom of the main for loop,
// which will perform a proper shutdown.
}

void xServer::OnRead( Connection* theConn, const string& line )
{
if( theConn != serverConnection )
	{
	elog	<< "xServer::OnRead> Unknown connection"
		<< endl ;
	return ;
	}

#ifdef EDEBUG
  burstLines++ ;
  burstBytes += line.size() ;
#endif

size_t len = line.size() - 1 ;
while( ('\n' == line[ len ]) || ('\r' == line[ len ]) )
	{
	--len ;
	}

memset( inputCharBuffer, 0, sizeof( inputCharBuffer ) ) ;
strncpy( inputCharBuffer, line.c_str(), len + 1 ) ;

if( verbose )
	{
	clog	<< "[IN]: "
		<< line ;
	}

#ifdef LOG_SOCKET
	socketFile	<< line ;
#endif

Process( inputCharBuffer ) ;

// Post the RAW read event
PostEvent( EVT_RAW, static_cast< void* >(
	const_cast< string* >( &line ) ) ) ;
}

// This function parses and distributes incoming lines
// of data
void xServer::Process( char* s )
{
if( (NULL == s) || (0 == s[ 0 ]) || (' ' == s[ 0 ]) )
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
//		elog	<< "xServer::Process> Handler failed for command: "
//			<< Command
//			<< ", args: "
//			<< Param
//			<< endl ;
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

//elog	<< "xServer::SquitServer> Searching for server "
//	<< serverName
//	<< endl ;

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
	<< reason ;

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
	const xClient* excludeMe )
{
// Make sure the event is valid.
if( !validEvent( theEvent ) )
	{
	elog	<< "xServer::PostEvent> Invalid event number: "
		<< theEvent
		<< endl ;
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
	if( (*ptr) != excludeMe )
		{
		(*ptr)->OnEvent( theEvent, Data1, Data2, Data3, Data4 ) ;
		}
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

// Let the client know it has been added to
// the server and its tables.
Client->ImplementServer( this ) ;

PostEvent( EVT_NICK, static_cast< void* >( theIClient ) ) ;

if( doBurst )
	{
	BurstClient( Client ) ;
	Client->BurstChannels() ;
	Client->BurstGlines() ;
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

elog	<< "xServer::DetachClient> Unable to find client moduleName: "
	<< moduleName
	<< endl ;

return false ;
}

void xServer::LoadClient( const string& moduleName,
	const string& configFileName )
{
//elog	<< "xServer::LoadClient("
//	<< moduleName
//	<< ", "
//	<< configFileName
//	<< ")"
//	<< endl ;

// First, unload the client.
// This will queue the request.
//UnloadClient( moduleName ) ;

string fileName = moduleName ;
if( '/' != fileName[ 0 ] )
	{
	// Relative path, prepend the libPrefix to the fileName
	// libPrefix is guaranteed to end with '/'
	fileName = libPrefix + moduleName ;
	}

// Next, queue the load request
LoadClientTimerHandler* handler = new (std::nothrow)
	LoadClientTimerHandler( this, fileName, configFileName ) ;
assert( handler != 0 ) ;

RegisterTimer( ::time( 0 ), handler, 0 ) ;
}

void xServer::UnloadClient( const string& moduleName,
	const string& reason )
{
//elog	<< "xServer::UnloadClient("
//	<< moduleName
//	<< ","
//	<< reason
//	<< ")> "
//	<< moduleName
//	<< endl ;

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
iClient* iClientPtr = theClient->getInstance() ;

// Notify each channel that the iClient has parted.
for( iClient::channelIterator chanItr = iClientPtr->channels_begin() ;
	chanItr != iClientPtr->channels_end() ; ++chanItr )
	{
	delete (*chanItr)->removeUser( iClientPtr ) ;

	if( (*chanItr)->empty() )
		{
		// The client was the last one in the channel
		delete Network->removeChannel( (*chanItr) ) ;
		}
	} // for()

// This is not strictly necessary, but serves to illustrate the
// internal client<->channel relationships.
iClientPtr->clearChannels() ;

// By this point, the xClient should have removed all of its
// custom data from each iClient in the network.
// Verify this.
void* customData = 0 ;
for( xNetwork::clientIterator clientItr = Network->clients_begin() ;
	clientItr != Network->clients_end() ; ++clientItr )
	{
	customData = clientItr->second->removeCustomData( theClient ) ;

	if( customData != 0 )
		{
//		elog	<< "xServer::removeClient> xClient "
//			<< *theClient
//			<< " forgot to remove customData for client "
//			<< *(clientItr->second)
//			<< endl ;
		}
	} // for()

// Walk the channelEventMap, and remove the xClient from all
// channel's in which it is registered.
// This will include channel "*"
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

// Remove all remaining timers for this xClient
removeAllTimers( theClient ) ;

// Close all Connections the xClient may have open/pending
ConnectionManager::Disconnect(
	reinterpret_cast< ConnectionHandler* >( theClient ), 0 ) ;

// ConnectionManager::Poll() must be called to perform the action
// that actually closes the above connections, but it is guaranteed
// to be called eventually, even if in doShutdown().

// Remove the client from the network data structures.
// Be sure to do this before closing the client's module,
// because closing the module will invalidate the client
// pointer.
Network->removeLocalClient( theClient ) ;

// Find this client in the module list
bool foundLocalClient = false ;
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

		foundLocalClient = true ;
		break ;
		}
	}

if( !foundLocalClient )
	{
	elog	<< "xServer::removeClient> Unable to find xClient: "
		<< *theClient
		<< endl ;
	}

// Deallocate the iClientPtr associated with this xClient
delete Network->removeClient( iClientPtr ) ;
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
if( !isConnected() )
	{
	return false ;
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
	if( useHoldBuffer )
		{
		burstHoldBuffer += buf + '\n' ;
		}
	else
		{
		serverConnection->Write( buf + '\n' ) ;
		}
	}
else
	{
	if( useHoldBuffer )
		{
		burstHoldBuffer += buf ;
		}
	else
		{
		serverConnection->Write( buf ) ;
		}
	}

// Return success.
return true ;
}

bool xServer::WriteDuringBurst( const string& buf )
{
// Is there a valid connection?
if( !isConnected() )
	{
	elog	<< "xServer::WriteDuringBurst> Not connected"
		<< endl ;
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
serverConnection->Write( buf ) ;

if( buf[ buf.size() - 1 ] != '\n' )
	{
	serverConnection->Write( string( "\n" ) ) ;
	}
return true ;
}

/**
 * Write the contents of a std::stringstream to the uplink connection.
 */
bool xServer::Write( const stringstream& s )
{
return Write( string( s.str() ) ) ;
}

bool xServer::WriteDuringBurst( const stringstream& s )
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
bool xServer::Write( const char* format, ... )
{
// Is there a valid connection?
if( !isConnected() )
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
if( verbose )
	{
	// Output the string to the console.
	cout << "[OUT]: " << buffer  ;
	}

	// Do we need to newline terminate it?
	if( buffer[ strlen( buffer ) - 1 ] != '\n' )
		{
		cout << endl ;
		}
#endif

if( buffer[ strlen( buffer ) - 1 ] != '\n' )
	{
	if( useHoldBuffer )
		{
		burstHoldBuffer += buffer ;
		burstHoldBuffer += "\n" ;
		}
	else
		{
		serverConnection->Write( buffer ) ;
		serverConnection->Write( string( "\n" ) ) ;
		}
	}
else
	{
	if( useHoldBuffer )
		{
		burstHoldBuffer += buffer ;
		}
	else
		{
		serverConnection->Write( buffer ) ;
		}
	}

return true ;
}

bool xServer::WriteDuringBurst( const char* format, ... )
{

// Is there a valid connection?
if( !isConnected() )
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
if( verbose )
	{
	// Output the string to the console.
	cout << "[OUT]: " << buffer  ;
	}

	// Do we need to newline terminate it?
	if( buffer[ strlen( buffer ) - 1 ] != '\n' )
		{
		cout << endl ;
		}
#endif

// Append the line to the output buffer.
serverConnection->Write( buffer ) ;
if( buffer[ strlen( buffer ) - 1 ] != '\n' )
	{
	serverConnection->Write( string( "\n" ) ) ;
	}

// Return success
return true ;
}

bool xServer::removeGline( const string& userHost, const xClient* remClient )
{
// This method is true if we find the gline in our internal
// structure of glines.
bool foundGline = false ;

// Perform a search for the gline
glineIterator gItr = findGlineIterator( userHost ) ;
if( gItr != gline_end() )
	{
	foundGline = true ;
	}

// Notify the network that we are removing it
// Even if we didn't find the gline here, it may be present
// to someone on the network *shrug*
stringstream s ;
s	<< charYY
	<< " GL * -"
	<< userHost ;

// Write the data to the network output buffer(s)
Write( s ) ;

// Did we find the gline in the interal gline structure?
if( foundGline )
	{
	// Remove the gline from the internal gline structure
	eraseGline( gItr ) ;

	// Let all clients know that the gline has been removed
	if( remClient )
		{
		PostEvent( EVT_REMGLINE,
			static_cast< void* >( gItr->second ), 
			0,0,0,remClient ) ;
		}
	else
		{
		PostEvent( EVT_REMGLINE,
			static_cast< void* >( gItr->second ) ) ;
		}
	// Deallocate the gline
	delete gItr->second ;
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
	const time_t& lastmod,
	const xClient* setClient,
	const string& server )
{
// Remove any old matches
{
	xServer::glineIterator gItr = findGlineIterator( userHost ) ;
	if( gItr != gline_end() )
		{
		// This gline is already present
		delete gItr->second ;
		eraseGline( gItr ) ;
		}
}

Gline* newGline =
	new (std::nothrow) Gline( setBy,
		userHost,
		reason,
		duration ,
		lastmod) ;
assert( newGline != 0 ) ;

// Notify the rest of the network
stringstream s ;
s	<< getCharYY() << " GL "
	<< server << " +"
	<< userHost << ' '
	<< duration << ' '
	<< lastmod << " :"
	<< reason ;
Write( s ) ;

glineList.insert( glineListType::value_type(
	newGline->getUserHost(), newGline ) ) ;
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
	if( !match( ptr->second->getUserHost(), userHost ) )
		{
		retMe.push_back( ptr->second ) ;
		}
	}

return retMe ;
}

const Gline* xServer::findGline( const string& userHost ) const
{
const_glineIterator gItr = glineList.find( userHost ) ;
if( gItr == glineList.end() )
	{
	return 0 ;
	}
return gItr->second ;
}

xServer::glineIterator xServer::findGlineIterator(
	const string& userHost )
{
return glineList.find( userHost ) ;
}

void xServer::addGline( Gline* newGline )
{
assert( newGline != 0 ) ;
glineList.insert( glineListType::value_type( newGline->getUserHost(),
	newGline ) ) ;
}

void xServer::sendGlinesToNetwork()
{
time_t now = ::time( 0 ) ;

for( const_glineIterator ptr = gline_begin() ;
	ptr != gline_end() ; ++ptr )
	{
	stringstream s ;
	s	<< getCharYY() << " GL * +"
		<< ptr->second->getUserHost() << ' '
		<< (ptr->second->getExpiration() - now) << ' '
		<< ptr->second->getLastmod() << " :"
		<< ptr->second->getReason() ;

	Write( s ) ;
	}
}

void xServer::removeMatchingGlines( const string& wildHost )
{
for( glineIterator ptr = gline_begin() ; ptr != gline_end() ; ++ptr )
	{
	// TODO: Does this work with two wildHost's?
	if( !match( wildHost, ptr->second->getUserHost() ) )
		{
		PostEvent( EVT_REMGLINE,
			static_cast< void* >( ptr->second ) ) ;

		delete ptr->second ;
		glineList.erase( ptr ) ;
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
	<< reason ;

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
//		<< chanName
//		<< endl ;

	// Create the channel
	// The client automatically gets op in this case
	{
	stringstream s ;
	s	<< theClient->getCharYYXXX()
		<< " C "
		<< chanName
		<< ' '
		<< postJoinTime ;
	Write( s ) ;
	}

	if( !chanModes.empty() )
		{
		stringstream s ;
		s	<< theClient->getCharYYXXX()
			<< " M "
			<< chanName << ' '
			<< chanModes ;
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

	Write( s ) ;
	}
else
	{
	// After bursting, and the channel exists
		{
		stringstream s2 ;
		s2	<< theClient->getCharYYXXX()
			<< " J "
			<< chanName ;

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
			<< theClient->getCharYYXXX() ;

		Write( s ) ;
		}

	if( !chanModes.empty() )
		{
		// Set the channel modes
		stringstream s ;
		s	<< theClient->getCharYYXXX() << " M "
			<< chanName << ' ' ;

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
	<< theClient->getDescription() ;
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

void xServer::BurstGlines()
{
xNetwork::localClientIterator ptr = Network->localClient_begin() ;
while( ptr != Network->localClient_end() )
	{
	(*ptr)->BurstGlines() ;
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
clog	<< "Number of glines: " << glineList.size() << endl ;
clog	<< "Last burst duration: " << (burstEnd - burstStart)
	<< " seconds" << endl ;
#ifdef EDEBUG
clog	<< "Read " << burstBytes
	<< " bytes and processed " << burstLines
	<< " commands over the last "
	<< (::time( 0 ) - burstStart)
	<< " seconds."
	<< endl ;
#endif
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
	void* data )
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
void xServer::OnChannelModeR( Channel* theChan, bool polarity,
	ChannelUser* sourceUser )
{
theChan->onModeR( polarity ) ;

// First deliver this channel event to any listeners for all channel
// events.
channelEventMapType::iterator allChanPtr =
	channelEventMap.find( CHANNEL_ALL ) ;
if( allChanPtr != channelEventMap.end() )
	{
	for( list< xClient* >::iterator ptr = allChanPtr->second->begin(),
		endPtr = allChanPtr->second->end() ; ptr != endPtr ; ++ptr )
		{
		(*ptr)->OnChannelModeR( theChan, polarity, sourceUser ) ;
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
	(*ptr)->OnChannelModeR( theChan, polarity, sourceUser ) ;
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
for( ; ptr != end ; ++ptr )
	{
	if( ptr->second->getExpiration() <= now )
		{
		// Expire the gline
		PostEvent( EVT_REMGLINE,
			static_cast< void* >( ptr->second ) ) ;

		delete ptr->second ;
		glineList.erase( ptr ) ;
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
	<< msg ;

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

void xServer::WriteBurstBuffer()
{
if( !isConnected() )
	{
	return ;
	}

serverConnection->Write( burstHoldBuffer.data() ) ;
burstHoldBuffer.clear() ;
}

void xServer::UserLogin( iClient* destClient, const string& account,
	xClient* sourceClient )
{
assert( destClient != 0 ) ;

if( account.empty() )
	{
	elog	<< "xServer::UserLogin> Empty account name/domain for user: "
		<< *destClient
		<< endl ;
	return ;
	}

destClient->setAccount( account ) ;

stringstream	outStream ;
outStream	<< getCharYY()
		<< " AC "
		<< destClient->getCharYYXXX()
		<< " "
		<< account ;
Write( outStream ) ;

PostEvent( EVT_ACCOUNT,
	static_cast< void* >( destClient ), 0, 0, 0,
	sourceClient ) ;
}

void xServer::setBursting( bool newVal )
{
bursting = newVal ;

#ifdef EDEBUG
if( newVal )
	{
	// Starting bursting
	burstBytes = burstLines = 0 ;
	}
else
	{
	// Completed bursting
	elog	<< "Completed net burst in "
		<< (burstEnd - burstStart)
		<< " seconds, read "
		<< burstBytes
		<< " bytes and processed "
		<< burstLines
		<< " commands"
		<< endl ;
	}
#endif
}

void xServer::removeAllTimers( TimerHandler* theHandler )
{
// Stack all timerInfo structures that do not correspond
// to the given TimerHandler.
std::stack< pair< time_t, timerInfo* > > theStack ;

while( !timerQueue.empty() )
	{
	pair< time_t, timerInfo* > thePair = timerQueue.top() ;
	timerQueue.pop() ;

	if( thePair.second->theHandler == theHandler )
		{
		// This timerInfo belongs to the TimerHandler in
		// question.
		elog	<< "xServer::removeAllTimers> Found "
			<< "a timer that was not unregistered"
			<< endl ;

		// Since the TimerHandler has little or no access
		// to the timer system internals here, it is safe
		// to simply call its OnTimerDestroy() method, even
		// though that method may call other xServer methods.
		theHandler->OnTimerDestroy( thePair.first, thePair.second ) ;

		delete thePair.second ;
		}
	else
		{
		// Add it to the stack
		theStack.push( thePair ) ;
		}
	 } // while( !empty )

// All done, now put the timers back into the timerQueue
while( !theStack.empty() )
	{
	timerQueue.push( theStack.top() ) ;
	theStack.pop() ;
	}
}

void xServer::doShutdown()
{
elog	<< "xServer::doShutdown> Removing modules..."
	<< endl ;

size_t count = 0 ;

// First, remove all clients
for( xNetwork::localClientIterator clientItr = Network->localClient_begin() ;
	clientItr != Network->localClient_end() ;
	++clientItr )
	{
	++count ;
	DetachClient( *clientItr, "Server shutdown" ) ;
	}
elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " local clients"
	<< endl ;

elog	<< "xServer::doShutdown> Removing network clients..."
	<< endl ;

count = 0 ;
// Clear the channels
while( Network->clients_begin() != Network->clients_end() )
	{
	++count ;
	iClient* theClient = Network->clients_begin()->second ;
	delete Network->removeClient( theClient ) ;
	}
elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " network clients"
	<< endl ;

elog	<< "xServer::doShutdown> Removing channels..."
	<< endl ;

count = 0 ;
while( Network->channels_begin() != Network->channels_end() )
	{
	++count ;
	Channel* theChan = Network->channels_begin()->second ;

	elog	<< "xServer::doShutdown> Found channel: "
		<< *theChan
		<< endl ;

	delete Network->removeChannel( theChan ) ;
	}
elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " channels"
	<< endl ;

elog	<< "xServer::doShutdown> Removing glines..."
	<< endl ;

count = 0 ;
// Remove glines
while( gline_begin() != gline_end() )
	{
	++count ;
	delete gline_begin()->second ;
	eraseGline( gline_begin() ) ;
	}
elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " glines"
	<< endl ;

elog	<< "xServer::doShutdown> Removing timers..."
	<< endl ;

count = 0 ;
// All of the client timers should be cleared, but let's verify
// The only timers left should be the server timers
while( !timerQueue.empty() )
	{
	++count ;
	elog	<< "xServer::doShutdown> Removing a timer"
		<< endl ;

	// Delete the timerInfo structure, but not the TimerHandler
	// or void* data.
	delete timerQueue.top().second ;
	timerQueue.pop() ;
	}
elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " timers"
	<< endl ;

elog	<< "xServer::doShutdown> Disconnecting..."
	<< endl ;

// Close the connection
if( serverConnection != 0 )
	{
	ConnectionManager::Disconnect( this, serverConnection ) ;
	}

// Perform the optional disconnect above.
// This will also commit the disconnects by any clients.
ConnectionManager::Poll() ;

// Deallocate the serverConnection
// The Connection is deallocated in ConnectionManager::Poll()
}

} // namespace gnuworld
