/**
 * winGateModule.cc
 * Copyright (C) 2002 Daniel Karrels (dan@karrels.com)  
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
 * $Id: wingateModule.cc,v 1.2 2003/06/28 01:21:21 dan_karrels Exp $
 */

#include	<string>

#include	"server.h"
#include	"client.h"
#include	"ConnectionManager.h"
#include	"ConnectionHandler.h"
#include	"Connection.h"
#include	"ScannerModule.h"
#include	"scanner.h"

namespace gnuworld
{

using std::string ;

/**
 * Wingate looking for:
 * "Wingate>"
 * "Too m"(any connections)
 */

class wingateModule : public ScannerModule
{

public:
	wingateModule( ConnectionManager* cm, scanner* theScanner ) ;
	virtual ~wingateModule() ;

	virtual void	CheckIP( const string& ip ) ;

	virtual void	OnConnect( Connection* ) ;
	virtual void	OnConnectFail( Connection* ) ;
	virtual void	OnRead( Connection*, const string& ) ;

} ;

wingateModule::wingateModule( ConnectionManager* cm,
	scanner* theScanner )
: ScannerModule( cm, theScanner )
{}

wingateModule::~wingateModule()
{}

void wingateModule::CheckIP( const string& ip )
{
cm->Connect( this, ip, 23 ) ;
}

void wingateModule::OnConnect( Connection* )
{
// Nothing to do, wait for input
}

void wingateModule::OnConnectFail( Connection* )
{
// Client ok
// No need to call CM::Disconnect(), it's already disconnected :)
}

void wingateModule::OnRead( Connection* cPtr, const string& line )
{
if( line == "Wingate>" )
	{
	// Open proxy
	theScanner->RejectClient( cPtr ) ;
	}

// Do nothing if the client is ok
cm->Disconnect( this, cPtr ) ;
}

} // namespace gnuworld
