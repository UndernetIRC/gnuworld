/**
 * msg_M351.cc
 */

#include	"server.h"
#include	"Network.h"
#include	"ELog.h"
#include	"xparameters.h"

const char msg_M351_cc_rcsId[] = "$Id: msg_M351.cc,v 1.2 2001/12/28 16:28:47 mrbean_ Exp $" ;
const char server_h_rcsId[] = __SERVER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;

namespace gnuworld
{

using std::endl ;

int xServer::MSG_M351( xParameters& Param )
{
if( Param.empty() )
	{
	elog	<< "xServer::MSG_M351> Invalid number of "
		<< "arguments"
		<< endl ;
	return -1 ;
	}

// Dont try this at home kids
char numeric[ 6 ] = { 0 } ;

xClient* theClient = Network->findLocalNick( Param[ 1 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_M351> Unable to find nick: "
		<< Param[ 1 ]
		<< endl ;
	return -1 ;
	}

strcpy( numeric, theClient->getCharYYXXX().c_str() ) ;
Param.setValue( 1, "351" ) ;
iServer* tmpServer = Network->findServerName(Param[0]);
if( NULL == tmpServer )
	{
	elog	<< "xServer::MSG_M351> Unable to find server: "
		<< Param[ 0 ]
		<< endl ;
	return -1 ;
	}
char numeric2[ 6 ] = { 0 } ;

strcpy( numeric2, tmpServer->getCharYY()) ;
Param.setValue( 0, numeric2 ) ;
string tMessage;
for(unsigned int i =0 ; i < Param.size();++i)
{
tMessage+=Param[i];
tMessage += " ";
}


return theClient->OnServerMessage(tmpServer,tMessage);
}

} // namespace gnuworld
