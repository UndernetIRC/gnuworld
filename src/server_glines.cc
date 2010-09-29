/**
 * server_glines.cc
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
 * $Id: server_glines.cc,v 1.3 2007/12/14 03:06:10 isomer Exp $
 */

#include	<new>
#include	<string>
#include	<vector>
#include	<sstream>
#include	<iostream>

#include	"server.h"
#include	"Network.h"
#include	"Gline.h"
#include	"ELog.h"

RCSTAG( "$Id: server_glines.cc,v 1.3 2007/12/14 03:06:10 isomer Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::vector ;
using std::endl ;
using std::stringstream ;

bool xServer::removeGline( const string& userHost, const xClient* remClient )
{
// This method is true if we find the gline in our internal
// structure of glines.
bool foundGline = false ;

// Perform a search for the gline
glineIterator gItr = findGlineIterator( userHost ) ;
if( gItr != glines_end() )
	{
	foundGline = true ;
	}

// Notify the network that we are removing it
stringstream s ;
if ( foundGline ) {
s	<< getCharYY()
	<< " GL * -"
	<< userHost 
	<< " "
	<< gItr->second->getExpiration() // expiration
	<< " "
	<< ::time(0)	// lastmod
	<< " "
	<< gItr->second->getExpiration() // expiration
	<< " :"
	<< gItr->second->getReason();
}
else {
// Even if we didn't find the gline here, it may be present
// to someone on the network *shrug*
s	<< getCharYY()
	<< " GL * -"
	<< userHost 
	<< " "
	<< ::time(0) + 60 // expire
	<< " "
	<< ::time(0)	// lastmod
	<< " "
	<< ::time(0) + 60 // lifetime
	<< " "
	<< ":Unknown G-Line";
}

// Write the data to the network output buffer(s)
Write( s ) ;

// Did we find the gline in the interal gline structure?
if( foundGline )
	{
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

	// Remove the gline from the internal gline structure
	eraseGline( gItr ) ;
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
	if( gItr != glines_end() )
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
	<< server << " !+"
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

for( const_glineIterator ptr = glines_begin() ;
	ptr != glines_end() ; ++ptr )
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

for( const_glineIterator ptr = glines_begin() ;
	ptr != glines_end() ; ++ptr )
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
for( glineIterator ptr = glines_begin() ; ptr != glines_end() ;
	++ptr )
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

void xServer::BurstGlines()
{
xNetwork::localClientIterator ptr = Network->localClient_begin() ;
while( ptr != Network->localClient_end() )
	{
	ptr->second->BurstGlines() ;
	++ptr ;
	}
}

void xServer::updateGlines()
{
time_t now = ::time( 0 ) ;

glineIterator	ptr = glines_begin();

while (ptr != glines_end())
{
	if( ptr->second->getExpiration() <= now )
		{
		// Expire the gline
		PostEvent( EVT_REMGLINE,
			static_cast< void* >( ptr->second ) ) ;

		delete ptr->second ;
		glineList.erase( ptr++ ) ;
		} else {
		ptr++;
		}
}
} // updateGlines()

} // namespace gnuworld
