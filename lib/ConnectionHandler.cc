/**
 * ConnectionHandler.cc
 * Author: Daniel Karrels (dan@karrels.com)
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
 * $Id: ConnectionHandler.cc,v 1.6 2002/06/02 23:14:23 dan_karrels Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>

#include	"ConnectionManager.h"
#include	"ConnectionHandler.h"
#include	"Connection.h"

const char ConnectionHandler_h_rcsId[] = __CONNECTIONHANDLER_H ;
const char ConnectionHandler_cc_rcsId[] = "$Id: ConnectionHandler.cc,v 1.6 2002/06/02 23:14:23 dan_karrels Exp $" ;
const char ConnectionManager_h_rcsId[] = __CONNECTIONMANAGER_H ;
const char Connection_h_rcsId[] = __CONNECTION_H  ;

namespace gnuworld
{

using std::stringstream ;
using std::string ;
using std::cout ;
using std::endl ;

ConnectionHandler::ConnectionHandler()
 : cmPtr( 0 )
{}

ConnectionHandler::ConnectionHandler( ConnectionManager* _cmPtr )
 : cmPtr( _cmPtr )
{}

ConnectionHandler::~ConnectionHandler()
{}

/**
 * These methods are empty.
 * Simply output a little debugging message if uncommented.
 */

void ConnectionHandler::OnConnect( Connection* cPtr )
{
cout	<< "ConnectionHandler::OnConnect> "
	<< *cPtr
	<< endl ;
}

void ConnectionHandler::OnConnectFail( Connection* cPtr )
{
cout	<< "ConnectionHandler::OnConnectFail> "
	<< *cPtr
	<< endl ;
}

void ConnectionHandler::OnRead( Connection* cPtr, const string& line )
{
cout	<< "ConnectionHandler::OnRead> "
	<< *cPtr
	<< ", line: "
	<< line
	<< endl ;
}

void ConnectionHandler::OnDisconnect( Connection* cPtr )
{
cout	<< "ConnectionHandler::OnDisconnect> "
	<< *cPtr
	<< endl ;
}

void ConnectionHandler::OnTimeout( Connection* cPtr )
{
cout	<< "ConnectionHandler::OnTimeout> "
	<< *cPtr
	<< endl ;
}

void ConnectionHandler::Write( Connection* cPtr, const string& msg )
{
// Public method, check method arguments
assert( cPtr != 0 ) ;

// cmPtr must first be set
assert( cmPtr != 0 ) ;

// Do nothing if the output message is empty, or the socket
// is a listening socket (not connected anyway).
if( msg.empty() || cPtr->isListening() )
	{
	return ;
	}

// Append the outgoing data onto the Connection's output buffer
cPtr->outputBuffer += msg ;
}

void ConnectionHandler::Write( Connection* cPtr, const stringstream& msg )
{
// Public method, check method arguments
assert( cPtr != 0 ) ;

// cmPtr must first be set
assert( cmPtr != 0 ) ;

// Do nothing if the output message is empty, or the socket
// is a listening socket (not connected anyway).
if( msg.str().empty() || cPtr->isListening() )
	{
	return ;
	}

// Append the outgoing data onto the Connection's output buffer
cPtr->outputBuffer += msg.str() ;
}

} // namespace gnuworld
