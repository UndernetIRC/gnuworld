/**
 * msg_PRIVMSG.cc
 */

#include	"server.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_PRIVMSG_cc_rcsId[] = "$Id: msg_PRIVMSG.cc,v 1.2 2001/02/06 00:26:00 dan_karrels Exp $" ;

using std::endl ;

namespace gnuworld
{

// This is a blatant hack until ircu gets its protocol straight
int xServer::MSG_PRIVMSG( xParameters& Param )
{
if( Param.empty() )
	{
	elog	<< "xServer::MSG_PRIVMSG> Invalid number of "
		<< "arguments"
		<< endl ;
	return -1 ;
	}

// Dont try this at home kids
char numeric[ 6 ] = { 0 } ;

iClient* theClient = Network->findNick( Param[ 0 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_PRIVMSG> Unable to find nick: "
		<< Param[ 0 ]
		<< endl ;
	return -1 ;
	}

strcpy( numeric, theClient->getCharYYXXX().c_str() ) ;
Param.setValue( 0, numeric ) ;

return MSG_P( Param ) ;
}

} // namespace gnuworld
