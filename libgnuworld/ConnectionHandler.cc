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
 * $Id: ConnectionHandler.cc,v 1.3 2003/08/05 01:59:14 dan_karrels Exp $
 */

#include	<string>
#include	<iostream>

#include	"ConnectionHandler.h"
#include	"Connection.h"

const char rcsId[] = "$Id: ConnectionHandler.cc,v 1.3 2003/08/05 01:59:14 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
using std::cout ;
using std::endl ;

/**
 * These methods are empty.
 * Simply output a little debugging message if uncommented.
 */

ConnectionHandler::ConnectionHandler()
{}

ConnectionHandler::~ConnectionHandler()
{}

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

void ConnectionHandler::OnRead( Connection*, const string& )
{
//cout	<< "ConnectionHandler::OnRead> "
//	<< *cPtr
//	<< ", line: "
//	<< line
//	<< endl ;
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

} // namespace gnuworld
