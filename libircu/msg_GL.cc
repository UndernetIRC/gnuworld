/**
 * msg_GL.cc
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
 * $Id: msg_GL.cc,v 1.1 2002/11/20 22:16:18 dan_karrels Exp $
 */

#include	<new>
#include	<iostream>

#include	<cassert>

#include	"server.h"
#include	"events.h"
#include	"Gline.h"
#include	"ELog.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"

const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char Gline_h_rcsId[] = __GLINE_H ;
const char events_h_rcsId[] = __EVENTS_H ;
const char msg_GL_cc_rcsId[] = "$Id: msg_GL.cc,v 1.1 2002/11/20 22:16:18 dan_karrels Exp $" ;

namespace gnuworld
{

using std::endl ;

CREATE_HANDLER(msg_GL)

/**
 * GLine message handler.
 * C GL * +~*@209.9.117.131 180 :Banned (~*@209.9.117.131) until 957235403
 *  (On Mon May 1 22:40:23 2000 GMT from SE5 for 180 seconds: remgline
 *  test.. 	[0])
 */
bool msg_GL::Execute( const xParameters& Params )
{
if( '-' == Params[ 2 ][ 0 ] )
	{
	// Removing a gline
	if( Params.size() < 3 )
		{
		elog	<< "msg_GL> Invalid number of arguments"
			<< endl ;
		return false ;
		}

	xServer::glineIterator ptr = theServer->gline_begin(),
		end = theServer->gline_end() ;
	for( ; ptr != end ; ++ptr )
		{
		if( (*ptr)->getUserHost() == (Params[ 2 ] + 1) )
			{
			// Found it
			break ;
			}
		}

	if( ptr == theServer->gline_end() )
		{
		// Gline not found
		elog	<< "msg_GL> Unable to find matching "
			<< "gline for removal: "
			<< Params[ 2 ]
			<< endl ;

		// Construct a faked Gline inorder to post it
		Gline* oldGline = new (std::nothrow)
			Gline( string(), Params[ 2 ] + 1,
				string(), 0 ) ;
		assert( oldGline != 0 ) ;

		theServer->PostEvent( EVT_REMGLINE,
			static_cast< void* >( oldGline ) ) ;
		delete oldGline; oldGline = 0 ;

		return false ;
		}

	theServer->PostEvent( EVT_REMGLINE,
		static_cast< void* >( *ptr ) ) ;

	theServer->eraseGline( ptr ) ;
	delete *ptr ;

	return true ;
	}

// Else, adding a gline
if( Params.size() < 5 )
	{
	elog	<< "msg_GL> Invalid number of arguments"
		<< endl ;
	return false ;
	}

Gline* newGline = 0;
xServer::glineIterator ptr = theServer->gline_begin(),
	end = theServer->gline_end() ;
for( ; ptr != end ; ++ptr )
	{
	if( !strcasecmp( (*ptr)->getUserHost(), Params[ 2 ] + 1 ) )
		{
		// Found it
		break ;
		}
	}

if( ptr == theServer->gline_end() )
	{
	newGline = new (std::nothrow) Gline(
		Params[ 0 ],
		Params[ 2 ] + 1,
		Params[ 4 ],
		atoi( Params[ 3 ] ) ) ;
	assert( newGline != 0 ) ;

	theServer->addGline( newGline ) ;
	}
else
	{
	newGline = *ptr;
	newGline->setExpiration(atoi(Params[ 3 ]) + ::time(0));
	newGline->setSetBy(Params [ 0 ] );
	newGline->setReason( Params [ 4 ] );
	}

theServer->PostEvent( EVT_GLINE,
	static_cast< void* >( newGline ) ) ;

return true ;
}

} // namespace gnuworld
