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

const char msg_GL_cc_rcsId[] = "$Id: msg_GL.cc,v 1.4 2002/01/08 18:39:35 mrbean_ Exp $" ;

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

if( '-' == Params[ 2 ][ 0 ] )
	{
	// Removing a gline
	if( Params.size() < 3 )
	{
	elog	<< "xServer::MSG_GL> Invalid number of arguments"
		<< endl ;
	return -1 ;
	}

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
		//Construct a faked Gline inorder to post it
		Gline* oldGline = new (std::nothrow) Gline(
			"",Params[2]+1,"",0);
		PostEvent( EVT_REMGLINE,
			static_cast< void* >( oldGline ) ) ;
		delete oldGline;
		return -1 ;
		}

	PostEvent( EVT_REMGLINE,
		static_cast< void* >( *ptr ) ) ;

	glineList.erase( ptr ) ;
	delete *ptr ;
	return 0 ;
	}

// Else, adding a gline
if( Params.size() < 5 )
	{
	elog	<< "xServer::MSG_GL> Invalid number of arguments"
		<< endl ;
	return -1 ;
	}

Gline* newGline;
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
	newGline = new (std::nothrow) Gline(
		Params[ 0 ],
		Params[ 2 ] + 1,
		Params[ 4 ],
		atoi( Params[ 3 ] ) ) ;
	assert( newGline != 0 ) ;

	glineList.push_back( newGline ) ;
	}
else
	{
	newGline = *ptr;
	newGline->setExpiration(atoi(Params[ 3 ]) + ::time(0));
	newGline->setSetBy(Params [ 0 ] );
	newGline->setReason( Params [ 4 ] );
	}
PostEvent( EVT_GLINE,
	static_cast< void* >( newGline ) ) ;

return 0 ;
}

} // namespace gnuworld
