/**
 * msg_GL.cc
 */

#include	<new>

#include	<cassert>

#include	"server.h"
#include	"events.h"
#include	"Gline.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_GL_cc_rcsId[] = "$Id: msg_GL.cc,v 1.3 2001/07/29 22:44:06 dan_karrels Exp $" ;

namespace gnuworld
{

using std::endl ;

/**
 * GLine message handler.
 * C GL * +~*@209.9.117.131 180 :Banned (~*@209.9.117.131) until 957235403
 *  (On Mon May 1 22:40:23 2000 GMT from SE5 for 180 seconds: remgline
 *  test.. 	[0])
 */
int xServer::MSG_GL( xParameters& Params )
{
if( Params.size() < 5 )
	{
	elog	<< "xServer::MSG_GL> Invalid number of arguments"
		<< endl ;
	return -1 ;
	}

if( '-' == Params[ 2 ][ 0 ] )
	{
	// Removing a gline
	glineListType::iterator ptr = glineList.begin(),
		end = glineList.end() ;
	for( ; ptr != end ; ++ptr )
		{
		if( (*ptr)->getUserHost() == (Params[ 2 ] + 1) )
			{
			// Found it
			break ;
			}
		}

	if( ptr == glineList.end() )
		{
		// Gline not found
		elog	<< "xServer::MSG_GL> Unable to find matching "
			<< "gline for removal: "
			<< Params[ 2 ]
			<< endl ;
		return -1 ;
		}

	glineList.erase( ptr ) ;
	delete *ptr ;
	return 0 ;
	}

// Else, adding a gline
Gline* newGline = new (std::nothrow) Gline(
	Params[ 0 ],
	Params[ 2 ] + 1,
	Params[ 4 ],
	atoi( Params[ 3 ] ) ) ;
assert( newGline != 0 ) ;

glineList.push_back( newGline ) ;
PostEvent( EVT_GLINE,
	static_cast< void* >( newGline ) ) ;

return 0 ;
}

} // namespace gnuworld
