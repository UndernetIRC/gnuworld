/**
 * ConnectionHandler.cc
 * Author: Daniel Karrels (dan@karrels.com)
 * $Id: ConnectionHandler.cc,v 1.3 2002/05/28 20:25:48 dan_karrels Exp $
 */

#include	<string>
#include	<iostream>

#include	"ConnectionHandler.h"
#include	"Connection.h"

using std::string ;
using std::cout ;
using std::endl ;

/**
 * These methods are empty.  There is no work to be done in
 * this class.  Simply output a little debugging message if
 * uncommented.
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
