/**
 * server_connection.cc
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
 * $Id: server_connection.cc,v 1.4 2006/12/22 06:41:45 kewlio Exp $
 */

#include	<new>
#include	<string>
//#include	<list>
//#include	<vector>
//#include	<algorithm>
#include	<sstream>
//#include	<fstream>
//#include	<stack>
//#include	<iostream>
//#include	<utility>

//#include	<cstdlib>
#include	<cstdio>
#include	<cstdarg>
//#include	<cstring>
//#include	<cassert>
//#include	<cerrno>
//#include	<csignal>

#include	"gnuworld_config.h"
//#include	"misc.h"
//#include	"events.h"
//#include	"ip.h"

#include	"misc.h"
#include	"server.h"
#include	"Network.h"
//#include	"iServer.h"
//#include	"iClient.h"
//#include	"EConfig.h"
//#include	"match.h"
#include	"ELog.h"
//#include	"StringTokenizer.h"
//#include	"xparameters.h"
//#include	"moduleLoader.h"
//#include	"ServerTimerHandlers.h"
//#include	"LoadClientTimerHandler.h"
//#include	"UnloadClientTimerHandler.h"
#include	"ConnectionManager.h"
#include	"ConnectionHandler.h"
#include	"Connection.h"

namespace gnuworld
{

//using std::pair ;
//using std::make_pair ;
using std::string ;
//using std::vector ;
//using std::list ;
using std::clog ;
using std::endl ;
using std::stringstream ;
//using std::stack ;
//using std::unary_function ;

void xServer::OnConnect( Connection* theConn )
{
// Just connected to our uplink
serverConnection = theConn ;

// P10 version information, bogus.
Version = 10 ;

// Set ourselves as a service.
me->setService() ;

// Set ourselves as IPv6.
me->setIPv6() ;

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
WriteDuringBurst( "SERVER %s %d %d %d J%02d %s +s6 :%s\n",
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

serverConnection = 0 ;
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
for( xNetwork::localClientIterator cItr = Network->localClient_begin() ;
	cItr != Network->localClient_end() ; ++cItr )
	{
	cItr->second->OnDisconnect() ;
	}
}

void xServer::OnRead( Connection* theConn, const string& line )
{
if( theConn != serverConnection )
	{
	elog	<< "xServer::OnRead> Unknown connection"
		<< endl ;
	return ;
	}

// Don't process any incoming data on the last iteration of
// the main control loop.
if( !keepRunning || lastLoop )
	{
	// Part of the shutdown process includes flushing any
	// data in the output buffer, and closing connections.
	// This requires calling Poll(), which will also
	// recv() and perform any distribution of messages to
	// handlers, including OnRead().
	// Therefore, only handle data if the server is still
	// in a running state.
	return ;
	}

burstLines++ ;
burstBytes += line.size() ;

size_t len = line.size() - 1 ;
while( ('\n' == line[ len ]) || ('\r' == line[ len ]) )
	{
	--len ;
	}

memset( inputCharBuffer, 0, sizeof( inputCharBuffer ) ) ;
strncpy( inputCharBuffer, line.c_str(), len + 1 ) ;

struct CommandMask {
	std::string command ;
	size_t pos ;
} ;

/* List of commands containing sensitive information to be redacted in logging.
 * 1. Command in upper case only.
 * 2. The position of the sensitive information in the command.
 */
static const CommandMask commandMasks[] = {
	{"LOGIN", 2},
	{"LOGIN2", 5},
	{"NEWPASS", 1},
	{"SUSPENDME", 1},
} ;

std::string logLine = line;
if( line.size() > 10
	&& ( ( line[ 6 ] == 'P' && line[ 9 ] == '@' )
	|| ( line[ 3 ] == 'X' && line[ 4 ] == 'Q' ) ) )
	{
	size_t colonPos = logLine.find(" :") ;
	if( colonPos != std::string::npos )
		{
		std::istringstream iss( logLine.substr( colonPos + 2 ) ) ;
		std::string word ;
		iss >> word ;

		// Find command in mask table
		for( const auto& mask : commandMasks )
			{
			if( string_upper(word) == mask.command )
				{
				std::vector< std::string > fields ;
				fields.push_back( word ) ;
				while( iss >> word ) fields.push_back( word ) ;

				// Scramble password if position exists
				if( fields.size() > mask.pos )
					fields[ mask.pos ] = gnuworld::mask( fields[ mask.pos ] ) ;

				// Reconstruct scrambled message
				std::ostringstream oss ;
				for( size_t i = 0 ; i < fields.size() ; ++i )
					{
					if( i > 0 ) oss << " " ;
					oss << fields[ i ] ;
					}
				std::string scrambled = oss.str() ;

				// If the original line ended with a newline, add it back
				if( !logLine.empty() && ( line.back() == '\n' || line.back() == '\r' ) )
					scrambled += "\n";

				logLine.replace( colonPos + 2, std::string::npos, scrambled ) ;
				break ;
			}
		}
	}
}

if( verbose )
	{
	clog	<< "[IN ]: "
		<< logLine ;
	}

if( logSocket )
	{
	socketFile	<< logLine ;
	socketFile.flush() ;
	}

Process( inputCharBuffer ) ;

// Post the RAW read event
PostEvent( EVT_RAW, static_cast< void* >(
	const_cast< string* >( &line ) ) ) ;
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
		clog << endl ;
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
		clog << endl ;
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

if( verbose )
	{
	// Output the string to the console.
	clog << "[OUT]: " << buffer  ;

	// Do we need to newline terminate it?
	if( buffer[ strlen( buffer ) - 1 ] != '\n' )
		{
		clog << endl ;
		}
	}

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

if( verbose )
	{
	// Output the string to the console.
	clog << "[OUT]: " << buffer  ;

	// Do we need to newline terminate it?
	if( buffer[ strlen( buffer ) - 1 ] != '\n' )
		{
		clog << endl ;
		}
	}

// Append the line to the output buffer.
serverConnection->Write( buffer ) ;
if( buffer[ strlen( buffer ) - 1 ] != '\n' )
	{
	serverConnection->Write( string( "\n" ) ) ;
	}

// Return success
return true ;
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

void xServer::FlushData()
{
if( !isConnected() )
	{
	return ;
	}
serverConnection->Flush() ;
}

} // namespace gnuworld
