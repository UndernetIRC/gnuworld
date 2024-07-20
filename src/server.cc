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
 * $Id: server.cc,v 1.226 2010/08/31 21:16:46 denspike Exp $
 */

#include	<sys/time.h>
#include	<unistd.h>

#include	<new>
#include	<string>
#include	<map>
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
#include	<cstring>
#include	<cassert>
#include	<cerrno>
#include	<csignal>
#include	<cstdarg>

#include	"gnuworld_config.h"
#include	"misc.h"
#include	"events.h"
#include	"ip.h"

#include	"server.h"
#include	"Network.h"
#include	"iServer.h"
#include	"iClient.h"
#include	"EConfig.h"
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

namespace gnuworld
{

using std::pair ;
using std::make_pair ;
using std::string ;
using std::vector ;
using std::list ;
using std::endl ;
using std::stringstream ;
using std::stack ;
using std::unary_function ;
using std::clog ;
using std::cout ;
using std::ends ;
using std::min ;

/// The object containing the network data structures
xNetwork*	Network = 0 ;

// Allocate the static std::string in xServer representing
// all channels.
const string xServer::CHANNEL_ALL( "*" ) ;

void xServer::initializeSystem()
{
initializeVariables() ;

clog	<< "*** Parsing configuration file "
	<< configFileName
	<< "..."
	<< endl ;
if( !readConfigFile( configFileName ) )
	{
	elog	<< "Error reading config file: "
		<< configFileName
		<< endl ;
	::exit( -1 ) ;
	}

// Output the information to the console.
elog	<< endl ;
elog	<< "Numeric: " << getIntYY()
	<< " ("
	<< getCharYY()
	<< ")"
	<< endl ;
elog	<< "Max Clients: " << getIntXXX()
	<< " ("
	<< getCharXXX()
	<< ")"
	<< endl ;
elog	<< "Uplink Name: " << UplinkName << endl ;
elog	<< "Uplink Port: " << Port << endl ;
elog	<< "Server Name: " << ServerName << endl ;
elog	<< "Server Description: " << ServerDescription << endl ;

//elog	<< "xServer::charYY> " << getCharYY() << endl ;
//elog	<< "xServer::charXXX> " << getCharXXX() << endl ;
//elog	<< "xServer::intYY> " << getIntYY() << endl ;
//elog	<< "xServer::intXXX> " << getIntXXX() << endl ;

me = new (std::nothrow) iServer(
	getIntYYXXX(),
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

Network->setServer( this ) ;

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
// All deallocations are performed in doShutdown()
elog.closeFile()  ;
if( logSocket )
	{
	socketFile.close() ;
	}
} // ~xServer()

void xServer::initializeVariables()
{
// Initialize more variables
keepRunning = true ;
bursting = false ;
sendEA = true ;
sendEB = true ;
lastLoop = false ;
useHoldBuffer = false ;
autoConnect = false ;
StartTime = ::time( NULL ) ;
shutDownReason = string( "Server Shutdown" ) ;

serverConnection = 0 ;
burstStart = burstEnd = 0 ;
Uplink = NULL ;
me = NULL ;
lastTimerID = 1 ;
glineUpdateInterval = pingUpdateInterval = 0 ;

Network = new (std::nothrow) xNetwork ;
assert( Network != 0 ) ;
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
setIntYY( atoi( conf.Require( "numeric" )->second.c_str() ) ) ;
setIntXXX( atoi( conf.Require( "maxclients" )->second.c_str() ) ) ;
commandMapFileName = conf.Require( "command_map" )->second ;

// autoConnect initialized to false
string strAutoConnect = conf.Require( "auto_reconnect" )->second ;
if( (strAutoConnect == "yes") || (strAutoConnect == "true") )
	{
	autoConnect = true ;
	}

iClient::setHiddenHostSuffix(
	conf.Require( "hidden_host_suffix" )->second ) ;

libPrefix = conf.Require( "libdir" )->second ;
if( libPrefix[ libPrefix.size() - 1 ] != '/' )
	{
	libPrefix += '/' ;
	}

// Load the control nickname(s), if any
EConfig::const_iterator cnItr = conf.Find( "controlnick" ) ;
for( ; (cnItr != conf.end()) && (cnItr->first == "controlnick") ;
	++cnItr )
	{
//	elog	<< "xServer> Adding control nickname: "
//		<< cnItr->second
//		<< endl ;
	controlNickSet.insert( cnItr->second ) ;
	}

// Load the control access list, AC usernames, if any
EConfig::const_iterator acItr = conf.Find( "allowcontrol" ) ;
for( ; (acItr != conf.end()) && (acItr->first == "allowcontrol") ;
	++acItr )
	{
//	elog	<< "xServer> Adding control authorization for username: "
//		<< acItr->second
//		<< endl ;

	allowControlSet.insert( acItr->second ) ;
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
			<< ", with symbol suffix: "
			<< st[ 1 ]
			<< endl ;
		returnVal = false ;
		break ;
		}

//	elog	<< "xServer::loadCommandHandlers> Loaded handler for "
//		<< st[ 2 ]
//		<< endl ;

	} // while()

elog	<< "Loaded "
	<< commandMap.size()
	<< " command handlers"
	<< endl
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
		<< ", with symbol suffix: "
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

//	elog	<< "xServer::loadClients> Found module: "
//		<< modInfo[0]
//		<< " (Config: "
//		<< modInfo[1]
//		<< ")"
//		<< endl;

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

/**
 * Initiate a server shutdown.
 */
void xServer::Shutdown( const string& reason )
{
lastLoop = true ;
autoConnect = false ;

// Set the shutdown reason before notifying clients, so each
// client can access the reason
setShutDownReason( reason ) ;

// Notify all xClients that a shutdown is being processed
for( xNetwork::localClientIterator itr = Network->localClient_begin() ;
	itr != Network->localClient_end() ; ++itr )
	{
	itr->second->OnShutdown( reason ) ;
	}

// Can't call removeClients() here because it is likely one of the
// clients that has invoked this call, that would be bad.
// Instead, the code that will halt the system is located in the
// main for() loop, which simply calls doShutdown() (private method).
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
bool xServer::SquitServer( const string& serverName,
	const string& reason )
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

/* Post Event
 * (Added Sept.26th 2011 by Hidden - Should fix a lag report bug in mod.ccontrol when a JUPE is issued)
 */
iServer* tmpServer = Network->findServerName(serverName);
if( NULL == tmpServer )
	{
	// The server doesn't exist.
	elog	<< "xServer::SquitServer> Unable to find server: "
		<< serverName
		<< endl ;
	return false ;
	}
string source(getCharYY());
string nreason(reason);
PostEvent(EVT_NETBREAK,
	static_cast<void *>(tmpServer),
	static_cast<void*>(&source),
	static_cast<void*>(&nreason));

         
iServer* theServer = Network->removeServer( tmpServer->getIntYY(), true ) ;
if( NULL == theServer )
	{
	// The server doesn't exist.
	elog	<< "xServer::SquitServer> Unable to find server: "
		<< serverName
		<< endl ;
	return false ;
	}

// Prepare the output buffer that will squit the server.
stringstream s ;
s	<< getCharYY()
	<< " SQ "
	<< serverName
	<< ' '
	<< theServer->getStartTime()
	<< " :"
	<< reason ;

// Notify the rest of the network of the SQUIT.
Write( s ) ;

// Deallocate the memory it occupies.
delete theServer ; theServer = 0 ;

// TODO: Log event
// TODO: Post message

// Squit successful.
return true ;
}

/**
 * Attach a server.  This could be either a jupe, or some fictitious
 * server from which to host virtual clients.
 */
bool xServer::AttachServer( iServer* fakeServer, xClient* owningClient )
{
assert( owningClient != 0 ) ;
assert( fakeServer != NULL ) ;

// Make sure a server of the same name is not already connected.
iServer* existingServer =
	Network->findServerName( fakeServer->getName() ) ;
if( existingServer != NULL )
	{
	// The server is already on the network.
	// Steal it's numeric :)
	fakeServer->setIntYY( existingServer->getIntYY() ) ;

	// Squit the old server and remove it from the internal tables.
	// This will also remove the server if it is already juped.
	SquitServer( existingServer->getName(),
		fakeServer->getDescription() ) ;

	// SquitServer() will also deallocate the server.
	// Make sure not to attempt to use the bogus tmp pointer.
	existingServer = 0 ;
	}

if( !Network->addFakeServer( fakeServer, owningClient ) )
	{
	elog	<< "xNetwork::AttachServer> Failed to attach fake "
		<< "server: "
		<< *fakeServer
		<< endl ;
	return false ;
	}

// Set the intXXX/charXXX to the max possible
fakeServer->setIntXXX( 64 * 64 * 64 - 1 ) ;

BurstServer( fakeServer ) ;

PostEvent( EVT_NETJOIN, static_cast< void* >( fakeServer ) ) ;

return true ;
}

void xServer::BurstServer( iServer* fakeServer )
{
assert( fakeServer != 0 ) ;

if( fakeServer->isJupe() )
	{
	// source_numeric JU +servername * expiration_time lastmod :reason
	// expiration: 604800 (max)
	Write( "%s JU * +%s 604800 %d :%s",
		getCharYY().c_str(),
		fakeServer->getName().c_str(),
		::time(0),fakeServer->getDescription().c_str() ) ;
	}
else
	{
	// Burst the new server's info./
	// IRCu checks for "JUPE " as being the beginning of the
	// reason as a jupe server.  This was because before servers
	// couldn't link without [ip] being added to their realname
	// field unless they were juped by uworld.  Now anyone can
	// link with that name, oh well.
	Write( "%s S %s %d %d %d J%02d %s 0 :%s\n",
			getCharYY().c_str(),
			fakeServer->getName().c_str(),
			2,
			0,
			fakeServer->getConnectTime(),
			10, // version
			fakeServer->getCharYYXXX().c_str(),
			fakeServer->getDescription().c_str() ) ;

	// Write burst acknowledgements.
	Write( "%s EB\n", fakeServer->getCharYY().c_str() ) ;
	Write( "%s EA\n", fakeServer->getCharYY().c_str() ) ;
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

// The xClient will be attached to this server.
Client->setIntYY( getIntYY() ) ;

// addClient() will allocate a new XXX and
// update Client.
if( !Network->addClient( Client ) )
	{
	elog	<< "xServer::AttachClient> Failed to update network tables"
		<< endl ;
	return false ;
	}

Client->MyUplink = this ;

// Let the client know it has been added to
// the server and its tables.
Client->OnAttach() ;

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
	string(),
	0,
	string(),
	string(),
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

PostEvent( EVT_NICK, static_cast< void* >( theIClient ) ) ;

if( doBurst )
	{
	BurstClient( Client ) ;
	Client->BurstChannels() ;
	Client->BurstGlines() ;
	}

elog	<< "Loaded client, nickname: "
	<< theIClient->getNickName()
	<< ", with config file: "
	<< Client->getConfigFileName()
	<< endl ;

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
xClient* clientPtr = NULL;
try {
// Attempt to instantiate an xClient instance from the module
clientPtr = ml->loadObject( configFileName );
} catch (...) {}

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
bool xServer::AttachClient( iClient* fakeClient,
	xClient* ownerClient )
{
assert( fakeClient != NULL ) ;
assert( ownerClient != 0 ) ;

// Verify that the iClient is in good order
if( fakeClient->getNickName().empty() ||
	fakeClient->getUserName().empty() ||
	fakeClient->getInsecureHost().empty() ||
	fakeClient->getDescription().empty() )
	{
	elog	<< "xServer::AttachClient(iClient)> Missing data "
		<< "in iClient: "
		<< *fakeClient
		<< endl ;
	return false ;
	}

// Let the xNetwork class handle filling in the information about the
// iClient.
if( !Network->addFakeClient( fakeClient, ownerClient ) )
	{
	elog	<< "xServer::AttachClient(iClient)> addFakeClient() "
		<< "failed"
		<< endl ;
	return false ;
	}

// Burst the client
BurstClient( fakeClient ) ;

return true ;
}

void xServer::BurstClient( iClient* fakeClient )
{
iServer* fakeServer = me ;
int hopCount = 1 ;
if( fakeClient->getIntYY() != getIntYY() )
	{
	hopCount = 2 ;
	fakeServer = Network->findFakeServer( fakeClient->getIntYY() ) ;
	assert( fakeServer != 0 ) ;
	}

string description( "Clone" ) ;
if( !fakeClient->getDescription().empty() )
	{
	description = fakeClient->getDescription() ;
	}

Write( "%s N %s %d %d %s %s %s %s %s :%s\n",
	fakeServer->getCharYY().c_str(),
	fakeClient->getNickName().c_str(),
	hopCount,
	fakeClient->getNickTS(),
	fakeClient->getUserName().c_str(),
	fakeClient->getInsecureHost().c_str(),
	fakeClient->getStringModes().c_str(),
	xIP( fakeClient->getIP() ).GetBase64IP().c_str(),
	fakeClient->getCharYYXXX().c_str(),
	description.c_str() ) ;

PostEvent( EVT_NICK, static_cast< void* >( fakeClient ) ) ;
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
Client->OnDetach( reason ) ;

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
//	elog	<< "xServer::DetachClient> moduleName: "
//		<< (*ptr)->getModuleName()
//		<< endl ;

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

RegisterTimer( ::time( 0 ) + 2, handler, 0 ) ;
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
//elog	<< "xServer::UnloadClient(xClient*)> "
//	<< theClient->getNickName()
//	<< endl ;

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
iClient* iClientPtr = Network->removeClient( theClient->getInstance() ) ;

PostEvent( EVT_QUIT, static_cast< void* >( iClientPtr ) ) ;

// Remove any fake clients and fake servers associated with this
// xClient.
list< iClient* > fakeClients = Network->findFakeClients( theClient ) ;
for( list< iClient* >::iterator cItr = fakeClients.begin() ;
	cItr != fakeClients.end() ; ++cItr )
	{
	iClient* fakeClient = *cItr ;

	// Issue the quite message for this client
	stringstream s ;
	s	<< fakeClient->getCharYYXXX()
		<< " Q :Exiting" ;
	Write( s ) ;

	PostEvent( EVT_QUIT, static_cast< void* >( fakeClient ) ) ;

	// Remove the fake client from all internal tables and
	// deallocate.  xNetwork::removeClient() will do all but
	// the deallocation.
	delete Network->removeClient( fakeClient ) ;
	} // for( cItr )

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

// Deallocate the iClient instance of the xClient
delete iClientPtr ; iClientPtr = 0 ;

// Reset the iClient instance for good measure
theClient->resetInstance() ;

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

void xServer::OnXQuery( iServer* theServer, const string& Routing,
                const string& Message)
{
void *const thisRouting = const_cast<char*>(Routing.c_str());
void *const thisMessage = const_cast<char*>(Message.c_str());
PostEvent( EVT_XQUERY,
	static_cast< void* >( theServer ),
	reinterpret_cast< void* >( thisRouting ),
	reinterpret_cast< void* >( thisMessage ) ) ;
}

void xServer::OnXReply( iServer* theServer, const string& Routing,
                const string& Message)
{
void *const thisRouting = const_cast<char*>(Routing.c_str());
void *const thisMessage = const_cast<char*>(Message.c_str());
PostEvent( EVT_XREPLY,
        static_cast< void* >( theServer ),
        reinterpret_cast< void* >( thisRouting ),
        reinterpret_cast< void* >( thisMessage ) ) ;
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

if( (theChan != 0) && (theChan->findUser( theClient->getInstance() )) )
	{
	elog    << "xServer::JoinChannel(xClient)> Client attempted "
		<< "to join channel "
		<< theChan->getName()
		<< " more than once"
		<< endl ;
	return false ;
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

//	elog	<< "xServer::JoinChannel> Creating new channel: "
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
		// Remove all modes to keep synced.
		removeAllChanModes( theChan ) ;
		}

	if( postJoinTime > theChan->getCreationTime() )
		{
		// We are bursting into a channel that has an
		// older timestamp than the one we were supplied with.
		// We use the existing timestamp to remain in sync
		// (And get op'd if needs be).
		postJoinTime = theChan->getCreationTime();
		}

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
			<< chanName
			<< " "
			<< postJoinTime ;

		Write( s2 ) ;
		}

	if( getOps )
		{
		// Op the bot
		stringstream s ;
		s	<< getCharYY()
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
			<< chanName << ' '
			<< chanModes ;
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
					theChan->setMode( Channel::MODE_T ) ;
				else
					theChan->removeMode( Channel::MODE_T ) ;
				break ;
			case 'n':
				if( plus )
					theChan->setMode( Channel::MODE_N ) ;
				else
					theChan->removeMode( Channel::MODE_N ) ;
				break ;
			case 's':
				if( plus )
					theChan->setMode( Channel::MODE_S ) ;
				else
					theChan->removeMode( Channel::MODE_S ) ;
				break ;
			case 'p':
				if( plus )
					theChan->setMode( Channel::MODE_P ) ;
				else
					theChan->removeMode( Channel::MODE_P ) ;
				break ;
			case 'm':
				if( plus )
					theChan->setMode( Channel::MODE_M ) ;
				else
					theChan->removeMode( Channel::MODE_M ) ;
				break ;
			case 'i':
				if( plus )
					theChan->setMode( Channel::MODE_I ) ;
				else
					theChan->removeMode( Channel::MODE_I ) ;
				break ;
			case 'r':
				if( plus )
					theChan->setMode( Channel::MODE_R ) ;
				else
					theChan->removeMode( Channel::MODE_R ) ;
				break ;
			case 'R':
				if( plus )
					theChan->setMode( Channel::MODE_REG ) ;
				else
					theChan->removeMode( Channel::MODE_REG ) ;
				break ;
			case 'D':
				if( plus )
					theChan->setMode( Channel::MODE_D ) ;
				else
					theChan->removeMode( Channel::MODE_D ) ;
				break ;
			case 'c':
				if (plus)
					theChan->setMode(Channel::MODE_C);
				else
					theChan->removeMode(Channel::MODE_C);
				break;
			case 'C':
				if (plus)
					theChan->setMode(Channel::MODE_CTCP);
				else
					theChan->removeMode(Channel::MODE_CTCP);
				break;
			case 'P':
				if (plus)
					theChan->setMode(Channel::MODE_PART);
				else
					theChan->removeMode(Channel::MODE_PART);
				break;
			case 'M':
				if (plus)
					theChan->setMode(Channel::MODE_MNOREG);
				else
					theChan->removeMode(Channel::MODE_MNOREG);
				break;

			// TODO: Finish with polarity
			// TODO: Add in support for modes b,v,o
			case 'k':
				{
				if( argPos >= st.size() )
					{
					elog	<< "xServer::JoinChannel> Invalid"
						<< " number of arguments to chanModes "
						<< " in 'k' ("
						<< chanModes.c_str()
						<< ")"
						<< endl ;
					break ;
					}
				/* if there is already a key, update it */
				if (theChan->getMode(Channel::MODE_K))
				{
					/* only update it if it is different! */
					if (strcmp(theChan->getKey().c_str(), st[argPos].c_str()))
					{
						Write("%s M %s -k %s\r\n",
							theClient->getCharYYXXX().c_str(),
							theChan->getName().c_str(),
							theChan->getKey().c_str());

						Write("%s M %s +k %s\r\n",
							theClient->getCharYYXXX().c_str(),
							theChan->getName().c_str(),
							st[argPos].c_str());
					}
				}
				theChan->onModeK( true, st[ argPos++ ] ) ;
				break ;
				}
			case 'A':
				{
				if( argPos >= st.size() )
					{
					elog	<< "xServer::JoinChannel> Invalid"
						<< " number of arguments to chanModes "
						<< " in 'A' ("
						<< chanModes
						<< ")"
						<< endl ;
					break ;
					}
				theChan->onModeA( true, st[ argPos++ ] ) ;
				break ;
				}
			case 'U':
				{
				if( argPos >= st.size() )
					{
					elog	<< "xServer::JoinChannel> Invalid"
						<< " number of arguments to chanModes "
						<< " in 'U' ("
						<< chanModes
						<< ")"
						<< endl ;
					break ;
					}
				theChan->onModeU( true, st[ argPos++ ] ) ;
				break ;
				}
			case 'l':
				{
				if( argPos >= st.size() )
					{
					elog	<< "xServer::JoinChannel> Invalid"
						<< " number of arguments to chanModes "
						<< " in 'l' ("
						<< chanModes
						<< ") for "
						<< theClient->getNickName()
						<< " in "
						<< chanName
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

PostChannelEvent( EVT_JOIN, theChan,
	static_cast< void* >( theIClient ),
	static_cast< void* >( theChanUser ) ) ;

theClient->OnJoin(theChan->getName());
return true ;
}

// K N Isomer 2 957217279 ~perry p136-tnt1.ham.ihug.co.nz DLbaCI KAC :*Unknown*
void xServer::BurstClient( xClient* theClient )
{
int hopCount = 1 ;
if( (theClient->getCharYY()[ 0 ] != getCharYY()[ 0 ]) ||
	(theClient->getCharYY()[ 1 ] != getCharYY()[ 1 ]) )
	{
	// The xClient is not on this server
	hopCount = 2 ;
	}

stringstream s ;
s	<< theClient->getCharYY() << " N "
	<< theClient->getNickName() << ' '
	<< hopCount << " 31337 "
	<< theClient->getUserName() << ' '
	<< theClient->getHostName() << ' '
	<< theClient->getModes() << ' '
	<< "AAAAAA "
	<< theClient->getCharYYXXX() << " :"
	<< theClient->getDescription() ;
Write( s ) ;

theClient->OnConnect() ;
}

void xServer::BurstClients()
{
xNetwork::localClientIterator ptr = Network->localClient_begin() ;
while( ptr != Network->localClient_end() )
	{
	BurstClient( ptr->second ) ;
	++ptr ;
	}
}

void xServer::BurstChannels()
{
xNetwork::localClientIterator ptr = Network->localClient_begin() ;
while( ptr != Network->localClient_end() )
	{
	ptr->second->BurstChannels() ;
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
	ptr->second->OnConnect() ;

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
clog	<< "Read " << burstBytes
	<< " bytes and processed " << burstLines
	<< " commands over the last "
	<< (::time( 0 ) - burstStart)
	<< " seconds."
	<< endl ;
}

void xServer::startLogging(bool logrotate)
{
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
	if (logrotate)
		socketFile.open( socketFileName.c_str(), std::ios::out | std::ios::app) ;
	else
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
}

void xServer::rotateLogs()
{
if (elog.isOpen())
	{
	elog << endl << "Received SIGHUP. Rotating log files..." << endl;
	//elog.closeFile();  /* Do not close the file, elog.openFile() will handle that.
	}
if( logSocket && socketFile.is_open() )
	{
	socketFile.close() ;
	}
startLogging(true);
}

void xServer::run()
{
mainLoop() ;
}

void xServer::removeAllChanModes( Channel* theChan )
{
modeVectorType modeVector ;

// This is a protected method, theChan is non-NULL
if( theChan->getMode( Channel::MODE_T ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_T ) ) ;
	}
if( theChan->getMode( Channel::MODE_N ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_N ) ) ;
	}
if( theChan->getMode( Channel::MODE_S ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_S ) ) ;
	}
if( theChan->getMode( Channel::MODE_P ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_P ) ) ;
	}
if( theChan->getMode( Channel::MODE_M ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_M ) ) ;
	}
if( theChan->getMode( Channel::MODE_I ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_I ) ) ;
	}
if( theChan->getMode( Channel::MODE_R ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_R ) ) ;
	}
if( theChan->getMode( Channel::MODE_REG ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_REG ) ) ;
	}
if( theChan->getMode( Channel::MODE_D ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_D ) ) ;
	}
if( theChan->getMode( Channel::MODE_C ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_C ) ) ;
	}
if( theChan->getMode( Channel::MODE_CTCP ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_CTCP ) ) ;
	}
if( theChan->getMode( Channel::MODE_PART ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_PART ) ) ;
	}
if( theChan->getMode( Channel::MODE_MNOREG ) )
	{
	modeVector.push_back( make_pair( false, Channel::MODE_MNOREG ) ) ;
	}
if( theChan->getMode( Channel::MODE_L ) )
	{
	OnChannelModeL( theChan, false, 0, 0 ) ;
	}
if( theChan->getMode( Channel::MODE_K ) )
	{
	OnChannelModeK( theChan, false, 0, string() ) ;
	}
if( theChan->getMode( Channel::MODE_A ) )
	{
	OnChannelModeA( theChan, false, 0, string() ) ;
	}
if( theChan->getMode( Channel::MODE_U ) )
	{
	OnChannelModeU( theChan, false, 0, string() ) ;
	}

if( !modeVector.empty() )
	{
	OnChannelMode( theChan, 0, modeVector ) ;
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

bool xServer::Mode( xClient* theClient,
	Channel* theChan,
	const string& modes,
	const string& args )
{
assert( theChan != 0 ) ;


/*
elog	<< "xServer::Mode> theChan: "
	<< *theChan
	<< ", modes: "
	<< modes
	<< ", args: "
	<< args
	<< ", MAX_CHAN_MODES: "
	<< MAX_CHAN_MODES
	<< endl ;
*/

// Make sure that the modes string is not empty, it's ok for
// the args string to be empty
if( modes.empty() )
	{
	return false ;
	}

// theUser is passed to the OnChannelMode*() methods below.
// This represents the user that is changing modes.
// If NULL, it represents that the server is changing modes.
// If non-NULL, it means that a client is changing modes.
// This if() will determine if the client or the server is changing
// modes, and set theUser appropriately.
// Assume the server is changing modes, and initialize to 0.
ChannelUser* theUser = 0 ;
if( theClient != 0 )
	{
	// The mode is being set as client, make sure the
	// client is in the channel, and is opped.

	// Return the iClient instance for the requesting xClient
	iClient* theIClient = theClient->getInstance() ;

	// Make sure the pointer is not NULL
	assert( theIClient != 0 ) ;

	// Attempt to find the ChannelUser for the requesting xClient
	// on the given channel.
	theUser = theChan->findUser( theIClient ) ;

	// Is the xClient in the channel?
	if( NULL == theUser )
		{
		// Nope, the xClient is not in the channel...silly xClient
		return false ;
		}

	// Is the xClient opped?
	if( !theUser->isModeO() )
		{
		// Nope, need to be opped to change modes.
		return false ;
		}
	} // if( theClient != 0 )

string modesAndArgsString( modes + ' ' + args ) ;

//elog	<< "xServer::Mode> modesAndArgsString: "
//	<< modesAndArgsString
//	<< endl ;

std::map< char, Channel::modeType > chanModes ;
chanModes[ 'i' ] = Channel::MODE_I ;
chanModes[ 'm' ] = Channel::MODE_M ;
chanModes[ 'n' ] = Channel::MODE_N ;
chanModes[ 'p' ] = Channel::MODE_P ;
chanModes[ 'r' ] = Channel::MODE_R ;
chanModes[ 'R' ] = Channel::MODE_REG ;
chanModes[ 's' ] = Channel::MODE_S ;
chanModes[ 't' ] = Channel::MODE_T ;
chanModes[ 'D' ] = Channel::MODE_D ;
chanModes[ 'c' ] = Channel::MODE_C ;
chanModes[ 'C' ] = Channel::MODE_CTCP ;
chanModes[ 'P' ] = Channel::MODE_PART ;
chanModes[ 'M' ] = Channel::MODE_MNOREG ;

// This vector is used for argument-less types that can be passed
// to OnChannelMode()
modeVectorType modeVector ;

opVectorType opVector ;
voiceVectorType voiceVector ;
banVectorType banVector ;

// This vector stores the actual output string for the modes
typedef std::vector< std::pair< std::string, std::string > >
	rawModeVectorType ;
rawModeVectorType rawModeVector ;

// generalModeVector and modeVector are used to ease the invocation
// of the various OnChannelMode*() methods, and rawModeVector is used
// to facilitate outputting the actual data to the network.

StringTokenizer st( modesAndArgsString ) ;
StringTokenizer::size_type tokenIndex = 0 ;

for( ; tokenIndex < st.size() ; )
	{
	bool polarityBool = true ;
	string polarityString( "+" ) ;

	// nextArgIndex is the index to the next mode argument.
	// This will be used to determine arguments for each mode,
	// as well as mark tokenIndex for the next loop.
	StringTokenizer::size_type nextArgIndex = tokenIndex + 1 ;

//	elog	<< "xServer::Mode> Evaluating tokenIndex "
//		<< tokenIndex
//		<< ", token: "
//		<< st[ tokenIndex ]
//		<< endl ;

	// Iterate this token, it may contain multiple modes
	for( size_t charIndex = 0 ; charIndex < st[ tokenIndex ].size() ;
		++charIndex )
		{
		char theChar = st[ tokenIndex ][ charIndex ] ;
//		elog	<< "xServer::Mode> Evaluating charIndex "
//			<< charIndex
//			<< ", theChar: "
//			<< theChar
//			<< endl ;

		switch( theChar )
			{
			case '+':
//				elog	<< "xServer::Mode> polarity = "
//					<< "true"
//					<< endl ;
				polarityBool = true ;
				polarityString = "+" ;
				break ;
			case '-':
//				elog	<< "xServer::Mode> polarity = "
//					<< "true"
//					<< endl ;
				polarityBool = false ;
				polarityString = "-" ;
				break ;
			case 'i':
			case 'm':
			case 'n':
			case 'p':
			case 'r':
			case 's':
			case 't':
			case 'c':
			case 'C':
			case 'P':
			case 'M':
			case 'D':
//				elog	<< "xServer::Mode> General mode: "
//					<< theChar
//					<< ", polarity: "
//					<< polarityString
//					<< endl ;
				modeVector.push_back( make_pair( 
					polarityBool,
					chanModes[ theChar ] ) ) ;
				rawModeVector.push_back( make_pair(
					polarityString + theChar,
					string() ) ) ;
				break ;
			case 'R':
					/* mod.cservice setting +R with a TS */
					if( nextArgIndex < st.size() && polarityBool ) {
						std::string TS = st[ nextArgIndex ] ;
						++nextArgIndex ;
                		                modeVector.push_back( make_pair(
                                		        polarityBool,
		                                        chanModes[ theChar ] ) ) ;
		                                rawModeVector.push_back( make_pair(
                		                        polarityString + theChar,
                                		        TS ) ) ;						
					} else {
						modeVector.push_back( make_pair(
		                                        polarityBool,
		                                        chanModes[ theChar ] ) ) ;
                		                rawModeVector.push_back( make_pair(
                                		        polarityString + theChar,
		                                        string() ) ) ;
					}
				break;
			case 'k':
				{
//				elog	<< "xServer::Mode> Mode 'k'"
//					<< ", polarity: "
//					<< polarityString
//					<< endl ;

				// Mode -k expects an argument, but it
				// doesn't matter what it is.

				// Must lookup the key argument
				if( nextArgIndex >= st.size() )
					{
					// No argument supplied
					return false ;
					}
				std::string chanKey = st[ nextArgIndex ] ;
				++nextArgIndex ;

				if( theChan->getMode( Channel::MODE_K ) )
					{
					// +k already set
					if( polarityBool )
						{
						// Must unset key first
						return false ;
						}
					// chanKey already handled
					}
				// If the mode is not set, just ignore
				// a polarity of "-k" (not add it to
				// to the modeVector)
				else
					{
					if( !polarityBool )
						{
						break ;
						}
					}

//				elog	<< "xServer::Mode> key: "
//					<< chanKey
//					<< endl ;

				OnChannelModeK(theChan, polarityBool,
					theUser, chanKey);
				rawModeVector.push_back( make_pair(
					polarityString + theChar,
					chanKey ) ) ;
				}
				break ;
			case 'A':
				{
//				elog	<< "xServer::Mode> Mode 'A'"
//					<< ", polarity: "
//					<< polarityString
//					<< end1 ;

				// Mode -A expects an argument, and it
				// matters what it is.
				// FIXFIX - TODO - XXX

				// Must lookup the Apass argument
				if( nextArgIndex >= st.size() )
					{
					// No argument supplied
					return false ;
					}
				std::string Apass = st[ nextArgIndex ] ;
				++nextArgIndex;

				if( theChan->getMode( Channel::MODE_A ) )
					{
					// +A already set
					if( polarityBool )
						{
						// Must unset Apass first
						return false ;
						}
					// Apass already handled
					}
				// If the mode is not set, just ignore
				// a polarity of "-A" (not add it to
				// the modeVector)
				else
				{
					if( !polarityBool )
						{
						break ;
						}
					}

//				elog	<< "xServer::Mode> Apass: "
//					<< Apass
//					<< endl ;

				OnChannelModeA(theChan, polarityBool,
					theUser, Apass);
				rawModeVector.push_back( make_pair(
					polarityString + theChar,
					Apass ) ) ;
				}
				break ;
			case 'U':
				{
//				elog	<< "xServer::Mode> Mode 'U'"
//					<< ", polarity: "
//					<< polarityString
//					<< endl ;

				// Mode -U expects an argument, and it
				// matters what it is.
				// FIXFIX - TODO - XXX

				// Must lookup the Upass argument
				if( nextArgIndex >= st.size() )
					{
					// No argument supplied
					return false ;
					}
				std::string Upass = st[ nextArgIndex ] ;
				++nextArgIndex ;

				if( theChan->getMode( Channel::MODE_U ) )
					{
					// +U already set
					if( polarityBool )
						{
						// Must unset Upass first
						return false ;
						}
					// Upass already handled
					}
				// If the mode is not set, just ignore
				// a polarity of "-U" (not add it to
				// the modeVector)
				else
					{
					if( !polarityBool )
						{
						break ;
						}
					}

//				elog	<< "xServer::Mode> Upass: "
//					<< Upass
//					<< endl ;

				OnChannelModeU(theChan, polarityBool,
					theUser, Upass);
				rawModeVector.push_back( make_pair(
					polarityString + theChar,
					Upass ) ) ;
				}
				break;
			case 'l':
				{
//				elog	<< "xServer::Mode> Mode 'l'"
//					<< ", polarity: "
//					<< polarityString
//					<< endl ;

				unsigned int chanLimit = 0 ;
				if( polarityBool )
					{
					// Must lookup the argument
					if( nextArgIndex >= st.size() )
						{
						// No argument supplied
						return false ;
						}
					chanLimit = ::atoi(
						st[ nextArgIndex ].c_str() ) ;
					++nextArgIndex ;
					}
				// No argument needed for -l

				OnChannelModeL(theChan, polarityBool,
					theUser, chanLimit);

				std::string chanLimitString ;
				if( chanLimit != 0 )
					{
					std::stringstream chanLimitSS ;
					chanLimitSS	<< chanLimit
							<< ends ;
					chanLimitSS	>> chanLimitString ;
					}

//				elog	<< "xServer::Mode> limit: "
//					<< chanLimit
//					<< ", chanLimitString: "
//					<< chanLimitString
//					<< endl ;

				rawModeVector.push_back( make_pair(
					polarityString + theChar,
					chanLimitString ) ) ;
				}
				break ;
			case 'b':
//				elog	<< "xServer::Mode> Mode 'b'"
//					<< ", polarity: "
//					<< polarityString
//					<< endl ;

				// Must lookup the ban argument
				if( nextArgIndex >= st.size() )
					{
					// No argument supplied
					return false ;
					}
//				elog	<< "xServer::Mode> ban: "
//					<< st[ nextArgIndex ]
//					<< endl ;

				banVector.push_back( make_pair(
					polarityBool, st[ nextArgIndex ]
					) ) ;
				rawModeVector.push_back( make_pair(
					polarityString + theChar,
					st[ nextArgIndex ] ) ) ;
				++nextArgIndex ;
				break ;
			case 'o':
			case 'v':
				{
//				elog	<< "xServer::Mode> Mode 'v'/'o'"
//					<< ", polarity: "
//					<< polarityString
//					<< endl ;

				// Must lookup the op/voice argument
				if( nextArgIndex >= st.size() )
					{
					// No argument supplied
					return false ;
					}

				iClient* targetClient =
					Network->findNick(
						st[ nextArgIndex ] ) ;
				if( 0 == targetClient )
					{
					return false ;
					}
				++nextArgIndex ;

				// Is the client on the channel?
				ChannelUser* targetUser =
					theChan->findUser( targetClient ) ;
				if( 0 == targetUser )
					{
					return false ;
					}

				// Make a few sanity checks.
				// The argument index has already been
				// updated, so no arguments will be
				// confused.
				if( 'o' == theChar )
					{
					if( targetUser->isModeO() && 
						polarityBool )
						{
						// Trying to op already opped 
						// user.
						break ;
						}
					if( !targetUser->isModeO() &&
						!polarityBool )
						{
						// Trying to deop user that is
						// not opped.
						break ;
						}
					} // if( 'o' )

				if( 'v' == theChar )
					{
					if( targetUser->isModeV() && 
						polarityBool )
						{
						// Trying to voice already 
						// voiced user.
						break ;
						}
					if( !targetUser->isModeV() &&
						!polarityBool )
						{
						// Trying to devoice user 
						// that is not voiced.
						break ;
						}
					} // if( 'v' )

				opVectorType::value_type thePair(
					polarityBool, targetUser ) ;
				if( 'v' == theChar )
					{
					voiceVector.push_back( thePair ) ;
					}
				else
					{
					opVector.push_back( thePair ) ;
					}

//				elog	<< "xServer::Mode> targetUser: "
//					<< *targetUser
//					<< endl ;

				rawModeVector.push_back(
					make_pair( polarityString + theChar,
					targetClient->getCharYYXXX() ) ) ;
				}
				break ; // 'o', 'v'
			} // switch()
		} // for( char )

	tokenIndex = nextArgIndex ;
	} // for( token )

size_t modeOutputCount = 0 ;
string outputModes ;
string outputArgs ;

// So now modeVector contains all of the modes, with arguments where
// appropriate, that need to be set.
for( rawModeVectorType::size_type modeIndex = 0 ;
	modeIndex < rawModeVector.size() ; )
	{
//	elog	<< "xServer::Mode> Iterating rawModeVector, modeIndex: "
//		<< modeIndex
//		<< ", outputModes: "
//		<< outputModes
//		<< ", outputArgs: "
//		<< outputArgs
//		<< endl ;

	// Each element in the rawModeVector is a <mode,arg> pair.
	// The for loop (directly) below is responsible for incrementing
	// modeIndex.
	for( ; (modeOutputCount < MAX_CHAN_MODES)
		&& (modeIndex < rawModeVector.size()) ;
		++modeOutputCount, ++modeIndex )
		{
		std::string polarityAndMode(
			rawModeVector[ modeIndex ].first ) ;
		std::string args = rawModeVector[ modeIndex ].second ;

/*
		elog	<< "xServer::Mode> modeOutputCount: "
			<< modeOutputCount
			<< ", modeIndex: "
			<< modeIndex
			<< ", polarityAndMode: "
			<< polarityAndMode
			<< ", args: "
			<< args
			<< endl ;
*/

		outputModes += polarityAndMode ;
		outputArgs += args + " " ;

//		elog	<< "xServer::Mode> outputModes: "
//			<< outputModes
//			<< ", outputArgs: "
//			<< outputArgs
//			<< endl ;
		} // modeOutputCount

	// Determine which part of the system is changing modes,
	// server or client.
	// Assume server.
	std::string modeSource( getCharYY() ) ;
	if( theClient != 0 )
		{
		// Change modes as the client
		modeSource = theClient->getCharYYXXX() ;
		}

	// In either of the cases which breaks out of the above for
	// loop, write the mode string.
	std::stringstream outputSS ;
	outputSS	<< modeSource
			<< " M "
			<< theChan->getName()
			<< " "
			<< outputModes
			<< " "
			<< outputArgs
			<< ends ;
/*	elog	<< "xServer::Mode> output: "
		<< outputSS.str()
		<< endl ; */
	Write( outputSS ) ;

	modeOutputCount = 0 ;
#if __GNUC__ == 2
	outputModes = "";
	outputArgs = "";
#else
	outputModes.clear();
	outputArgs.clear();
#endif
	} // for( modeItr )

// Distribute events for the argument-less modes
if( !modeVector.empty() )
	{
	OnChannelMode( theChan, theUser, modeVector ) ;
	}
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

return true ;
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

void xServer::UserLogin( iClient* destClient, const string& account,
	const time_t account_id, xClient* sourceClient )
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
destClient->setAccountID(account_id);

stringstream	outStream ;
outStream	<< getCharYY()
		<< " AC "
		<< destClient->getCharYYXXX()
		<< " "
		<< account
		<< " "
		<< account_id;
Write( outStream ) ;

PostEvent( EVT_ACCOUNT,
	static_cast< void* >( destClient ), 0, 0, 0,
	sourceClient ) ;
}

void xServer::setBursting( bool newVal )
{
bursting = newVal ;

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
}

void xServer::doShutdown()
{
//elog	<< "xServer::doShutdown> Removing modules..."
//	<< endl ;

size_t count = 0 ;

// First, remove all clients
for( xNetwork::localClientIterator clientItr = Network->localClient_begin() ;
	clientItr != Network->localClient_end() ; )
	{
	++count ;
	DetachClient( clientItr++->second, "Server shutdown" ) ;
	}

elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " local clients"
	<< endl ;

//elog	<< "xServer::doShutdown> Removing network clients..."
//	<< endl ;

count = 0 ;
// Clear the channels
for( xNetwork::clientIterator cItr = Network->clients_begin() ;
	cItr != Network->clients_end() ; )
	{
	iClient* theClient = cItr->second ;
	++count ;
	++cItr ;
	delete Network->removeClient( theClient ) ;
	}
elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " network clients"
	<< endl ;

//elog	<< "xServer::doShutdown> Removing channels..."
//	<< endl ;

count = 0 ;
for( xNetwork::channelIterator cItr = Network->channels_begin() ;
	cItr != Network->channels_end() ; )
	{
	Channel* theChan = cItr->second ;
	++count ;
	++cItr ;

	elog	<< "xServer::doShutdown> Found channel: "
		<< *theChan
		<< endl ;

	delete Network->removeChannel( theChan ) ;
	}
elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " channels"
	<< endl ;

// Remove servers
count = 0 ;
while (Network->serverList_size() > 0)
{
	xNetwork::serverIterator sItr = Network->servers_begin();
	++count;

	iServer* tmpServer = sItr->second ;
	delete Network->removeServer( tmpServer->getIntYY() ) ;
}

elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " servers..."
	<< endl ;

//elog	<< "xServer::doShutdown> Removing glines..."
//	<< endl ;

count = 0 ;
// Remove glines
for( glineIterator gItr = glines_begin() ; gItr != glines_end() ; )
	{
	Gline *tmpGline = gItr->second ;
	++count ;
	eraseGline( gItr++ ) ;
	delete tmpGline ;
	}
elog	<< "xServer::doShutdown> Removed "
	<< count
	<< " glines"
	<< endl ;

//elog	<< "xServer::doShutdown> Removing timers..."
//	<< endl ;

count = 0 ;
// All of the client timers should be cleared, but let's verify
// The only timers left should be the server timers
while( !timerQueue.empty() )
	{
	++count ;
//	elog	<< "xServer::doShutdown> Removing a timer"
//		<< endl ;

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

bool xServer::DetachClient( iClient* fakeClient,
	const string& quitMessage )
{
assert( fakeClient != 0 ) ;

// xNetwork::removeFakeClient() will remove the client from
// the network data structurs, and free its numeric
if( 0 == Network->removeClient( fakeClient ) )
	{
	elog	<< "xNetwork::DetachClient(iClient)> Failed to remove "
		<< "fakeClient from network data structures: "
		<< *fakeClient
		<< endl ;
	return false ;
	}

if( !quitMessage.empty() )
	{
	Write( "%s Q :%s",
		fakeClient->getCharYYXXX().c_str(),
		quitMessage.c_str() ) ;
	}
else
	{
	Write( "%s Q :Exiting",
		fakeClient->getCharYYXXX().c_str() ) ;
	}

PostEvent( EVT_QUIT, static_cast< void* >( fakeClient ) ) ;

return true ;
}

bool xServer::DetachServer( iServer* fakeServer )
{
assert( fakeServer != 0 ) ;

if( 0 == Network->removeServer( fakeServer->getIntYY() ) )
	{
	elog	<< "xServer::DetachServer> Failed to remove server: "
		<< *fakeServer
		<< endl ;
	return false ;
	}

if( fakeServer->isJupe() )
	{
	// source_numeric JU -servername * expiration_time lastmod :reason
	// expiration: 604800 (max)
	Write( "%s JU * -%s  604800 %d :%s",
		getCharYY().c_str(),
		fakeServer->getName().c_str(),
		::time(0),fakeServer->getDescription().c_str() ) ;
	}

else
	{
	Write( "%s SQ %s %d :Unloading server",
		getCharYY().c_str(),
		fakeServer->getCharYY().c_str(),
		fakeServer->getConnectTime() ) ;
	}

return true ;
}

bool xServer::JoinChannel( iClient* theClient, const string& chanName )
{
assert( theClient != 0 ) ;

if( 0 == Network->findFakeClient( theClient ) )
	{
	// Not a fake client
	elog	<< "xServer::JoinChannel (fake)> Attempt to force a "
		<< "non-fake client to join a channel: "
		<< *theClient
		<< endl ;

	return false ;
	}

Channel* theChan = Network->findChannel( chanName ) ;
if( 0 == theChan )
	{
	elog	<< "xServer::JoinChannel (fake)> Attempting to join "
		<< "non-existing channel: "
		<< chanName
		<< endl ;
	return false ;
	}

// If the user is already on the channel, don't rejoin
if( theChan->findUser( theClient ) )
	{
	// User already in channel
	return true ;
	}

ChannelUser* theUser = new (std::nothrow) ChannelUser( theClient ) ;
assert( theUser != 0 ) ;

if( !theChan->addUser( theUser ) )
	{
	elog	<< "xServer::JoinChannel (fake)> Failed to add user "
		<< "to channel: "
		<< *theChan
		<< endl ;
	delete theUser ; theUser = 0 ;
	return false ;
	}

if( !theClient->addChannel( theChan ) )
	{
	elog	<< "xServer::JoinChannel (fake)> Failed to add channel "
		<< "to client: "
		<< *theClient
		<< endl ;

	theChan->removeUser( theUser ) ;
	delete theUser ; theUser = 0 ;
	return false ;
	}

stringstream s ;
s	<< theClient->getCharYYXXX()
	<< " J "
	<< chanName
	<< ' '
	<< ::time( 0 ) ;
Write( s ) ;

PostChannelEvent( EVT_JOIN, theChan,
	static_cast< void* >( theClient ),
	static_cast< void* >( theUser ) ) ;

return true ;
}

void xServer::PartChannel( iClient* theClient, const string& chanName,
	const string& reason )
{
assert( theClient != 0 ) ;

if( 0 == Network->findFakeClient( theClient ) )
	{
	// Not a fake client
	elog	<< "xServer::PartChannel (fake)> Attempt to force a "
		<< "non-fake client to part a channel: "
		<< *theClient
		<< endl ;

	return ;
	}

Channel* theChan = Network->findChannel( chanName ) ;
if( 0 == theChan )
	{
	elog	<< "xServer::ParChannel (fake)> Attempting to part "
		<< "non-existing channel: "
		<< chanName
		<< endl ;
	return ;
	}

// Perform both operations below regardless of the return values,
// just to ensure that all parts are in synch
delete theChan->removeUser( theClient ) ;
theClient->removeChannel( theChan ) ;

stringstream s ;
s	<< theClient->getCharYYXXX()
	<< " L "
	<< chanName ;

if( !reason.empty() )
	{
	s	<< " :"
		<< reason ;
	}
Write( s ) ;

PostChannelEvent( EVT_PART, theChan,
	static_cast< void* >( theClient ) ) ;
}

/// Have the server burst a channel
bool xServer::BurstChannel( const string& chanName,
	const string& chanModes,
	const time_t& burstTime )
{
// Only send a B command during bursting
if( !bursting )
	{
	return false ;
	}

// chanModes cannot contain a '-' mode, but can contain
// a key that has '-' in it
StringTokenizer st( chanModes ) ;
if( st[ 0 ].find( '-' ) != string::npos )
	{
	elog	<< "xServer::BurstChannel> Channel modes cannot "
		<< "contain a \'-\' polarity modifier"
		<< endl ;
	return false ;
	}

// Since we are not bursting a client, the channel must exist
// already for this to make any sense.
Channel* theChan = Network->findChannel( chanName ) ;
if( 0 == theChan )
	{
	elog	<< "xServer::BurstChannel> Channel does not exist: "
		<< chanName
		<< endl ;
	return false ;
	}

if( burstTime >= theChan->getCreationTime() )
	{
	elog	<< "xServer::BurstChannel> Channel creation time is "
		<< "older than the burst time"
		<< endl ;
	return false ;
	}

theChan->removeAllModes() ;
theChan->removeAllBans() ;

// Need to burst the channel
stringstream s ;
s	<< getCharYY()
	<< " B "
	<< chanName << ' '
	<< burstTime << ' '
	<< chanModes  ;

Write( s ) ;

theChan->setCreationTime( burstTime ) ;

if( chanModes.empty() ||
	(string::npos == chanModes.find_first_not_of( ' ' )))
	{
	// No problem
	return true ;
	}

StringTokenizer::size_type argPos = 1 ;

for( string::const_iterator ptr = st[ 0 ].begin() ; ptr != st[ 0 ].end() ; 
	++ptr )
	{
	switch(  *ptr )
		{
		case 't':
			theChan->setMode( Channel::MODE_T ) ;
			break ;
		case 'n':
			theChan->setMode( Channel::MODE_N ) ;
			break ;
		case 's':
			theChan->setMode( Channel::MODE_S ) ;
			break ;
		case 'p':
			theChan->setMode( Channel::MODE_P ) ;
			break ;
		case 'm':
			theChan->setMode( Channel::MODE_M ) ;
			break ;
		case 'i':
			theChan->setMode( Channel::MODE_I ) ;
			break ;
		case 'r':
			theChan->setMode( Channel::MODE_R ) ;
			break ;
		case 'R':
			theChan->setMode( Channel::MODE_REG ) ;
			break ;
		case 'D':
			theChan->setMode( Channel::MODE_D ) ;
			break ;
		case 'c':
			theChan->setMode( Channel::MODE_C ) ;
			break;
		case 'C':
			theChan->setMode( Channel::MODE_CTCP ) ;
			break;
		case 'P':
			theChan->setMode( Channel::MODE_PART ) ;
			break;
		case 'M':
			theChan->setMode( Channel::MODE_MNOREG ) ;
			break;
		case 'k':
			{
			if( argPos >= st.size() )
				{
				elog	<< "xServer::BurstChannel> Invalid"
					<< " number of arguments to "
					<< "chanModes"
					<< endl ;
				break ;
				}
			theChan->onModeK( true, st[ argPos++ ] ) ;
			break ;
			}
		case 'A':
			{
			if( argPos >= st.size() )
				{
				elog	<< "xServer::BurstChannel> Invalid"
					<< " number of arguments to "
					<< "chanModes"
					<< endl ;
				break ;
				}
			theChan->onModeA( true, st[ argPos++ ] ) ;
			break ;
			}
		case 'U':
			{
			if( argPos >= st.size() )
				{
				elog	<< "xServer::BurstChannel> Invalid"
					<< " number of arguments to "
					<< "chanModes"
					<< endl ;
				break ;
				}
			theChan->onModeU( true, st[ argPos++ ] ) ;
			break ;
			}
		case 'l':
			{
			if( argPos >= st.size() )
				{
				elog	<< "xServer::BurstChannel> Invalid"
					<< " number of arguments to "
					<< "chanModes"
					<< endl ;
				break ;
				}
			theChan->onModeL( true,
				atoi( st[ argPos++ ].c_str() ) ) ;
			break ;
			}
		default:
			break ;
		} // switch()
	} // for()

return true ;
}

bool xServer::findControlNick( const std::string& nickName ) const
{
//elog	<< "xServer::findControlNick> nickName: "
//	<< nickName
//	<< endl ;
return (controlNickSet.find( nickName ) != controlNickSet.end()) ;
}

void xServer::ControlCommand( iClient* srcClient,
	const string& message )
{
assert( srcClient != 0 ) ;

elog	<< "xServer::ControlCommand> Received control message from: "
	<< *srcClient
	<< ": "
	<< message
	<< endl ;

if( !hasControlAccess( srcClient->getAccount() ) ||
	!srcClient->isOper() )
	{
	// Silently return
	return ;
	}

StringTokenizer msgTokens( message ) ;
if( msgTokens.size() < 2 )
	{
	Notice( srcClient, "Unable to comply" ) ;
	return ;
	}

// This is hideously ugly, if there proves to be any significant
// need for many commands supported by the server then I will
// use something with a better OO design (ok, so it will probably
// get done just out of embarassment)
const string command = string_lower( msgTokens[ 0 ] ) ;

// For unloadclient, the full path to the library must be specified,
// for example: /home/gnuworld/gnuworld/lib/libstats.la
if( msgTokens[ 0 ] == "unloadclient" )
	{
	string reason ;
	if( msgTokens.size() >= 3 )
		{
		reason = msgTokens.assemble( 2 ) ;
		}
	UnloadClient( msgTokens[ 1 ], reason ) ;
	Notice( srcClient, string( "Attempting to unload client: " )
		+ msgTokens[ 1 ] ) ;
	return ;
	}

if( msgTokens.size() < 3 )
	{
	Notice( srcClient, "Unable to comply" ) ;
	return ;
	}

if( msgTokens[ 0 ] == "loadclient" )
	{
	LoadClient( msgTokens[ 1 ], msgTokens[ 2 ] ) ;
	Notice( srcClient, string( "Attempting to load client module: " )
		+ msgTokens[ 1 ] + ", with config file: "
		+ msgTokens[ 2 ] ) ;
	return ;
	}
}

void xServer::OnTimeout( Connection* cPtr )
{
std::stringstream s;
s	<< "xServer::OnTimeout> "
	<< *cPtr
	<< endl ;
cout << s.str();
if (cPtr == serverConnection)
	{
	/* If the connection timeout happens on the hub connection, stop the main loop in main.cc */
	lastLoop = true;
	elog << s.str();  /* Line not added to the debug log file - possibly because the program exits quickly because of lastLoop. Didn't verify */
	}
}

bool xServer::hasControlAccess( const std::string& userName ) const
{
return (allowControlSet.find( userName ) != allowControlSet.end()) ;
}

bool xServer::Notice( iClient* theClient, const string& message )
{
assert( theClient != 0 ) ;

if( message.empty() || !isConnected() )
	{
	return false ;
	}

stringstream s ;
s	<< getCharYY()
	<< " O "
	<< theClient->getCharYYXXX()
	<< " :"
	<< message ;
return Write( s.str() ) ;
}

bool xServer::Notice( iClient* theClient, const char* format, ... )
{
assert( theClient != 0 ) ;

char buf[ 1024 ] = { 0 } ;
va_list _list ;

va_start( _list, format ) ;
vsnprintf( buf, 1024, format, _list ) ;
va_end( _list ) ;

stringstream s ;
s	<< getCharYY()
	<< " O "
	<< theClient->getCharYYXXX()
	<< " :"
	<< buf
	<< ends ;

return Write( s );
}

bool xServer::serverNotice( Channel* theChan, const char* format, ... )
{
	assert( theChan != 0 ) ;

	char buf[ 1024 ] = { 0 } ;
	va_list _list ;

	va_start( _list, format ) ;
	vsnprintf( buf, 1024, format, _list ) ;
	va_end( _list ) ;

	stringstream s;
	s	<< getCharYY()
		<< " O "
		<< theChan->getName()
		<< " :"
		<< buf
		<< ends;

	return Write( s );
}

bool xServer::serverNotice( Channel* theChan, const string& Message)
{
	assert( theChan != 0 ) ;

	if( Message.empty() || !isConnected() )
	{
		return false ;
	}

	stringstream s;
	s	<< getCharYY()
		<< " O "
		<< theChan->getName()
		<< " :"
		<< Message
		<< ends;

	return Write( s );
}

bool xServer::XReply (iServer* theServer, const string& Routing,
	const string& Message)
{
assert (theServer != 0 );

if( Message.empty() || !isConnected() )
        {
        return false ;
        }
stringstream s ;
s       << getCharYY()
        << " XR "
        << theServer->getCharYY()
        << " "
	<< Routing
	<< " :"
        << Message ;
return Write( s.str() ) ;
	
}

bool xServer::XQuery (iServer* theServer, const string& Routing,
        const string& Message)
{
assert (theServer != 0 );

if( Message.empty() || !isConnected() )
        {
        return false ;
        }
stringstream s ;
s       << getCharYY()
        << " XQ "
        << theServer->getCharYY()
        << " "
        << Routing
        << " :"
        << Message ;
return Write( s.str() ) ;

}
} // namespace gnuworld
