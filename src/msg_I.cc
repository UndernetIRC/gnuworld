/**
 * msg_I.cc
 */

#include	<iostream>

#include	"server.h"
#include	"xparameters.h"
#include	"Channel.h"
#include	"iClient.h"
#include	"ELog.h"
#include	"client.h"
#include	"Network.h"

const char server_h_rcsId[] = __SERVER_H ;
const char xparameters_h_rcsId[] = __XPARAMETERS_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char Channel_h_rcsId[] = __CHANNEL_H ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char client_h_rcsId[] = __CLIENT_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char msg_I_cc_rcsId[] = "$Id: msg_I.cc,v 1.2 2002/04/28 16:11:23 dan_karrels Exp $" ;

namespace gnuworld
{

using std::endl ;

// ABAHo I X :#lksdlkj
int xServer::MSG_I( xParameters& Param )
{
if( Param.size() != 3 )
	{
	elog	<< "xServer::MSG_I> Invalid number of arguments"
		<< endl ;
	return -1 ;
	}

iClient* srcClient = Network->findClient( Param[ 0 ] ) ;
if( NULL == srcClient )
	{
	elog	<< "xServer::MSG_I> Unable to find source client: "
		<< Param[ 0 ]
		<< endl ;
	return -1 ;
	}

xClient* destClient = Network->findLocalNick( Param[ 1 ] ) ;
if( NULL == destClient )
	{
	elog	<< "xServer::MSG_I> Unable to find destination client: "
		<< Param[ 1 ]
		<< endl ;
	return -1 ;
	}

Channel* theChan = Network->findChannel( Param[ 2 ] ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::MSG_I> Unable to find channel: "
		<< Param[ 2 ]
		<< endl ;
	return -1 ;
	}

return destClient->OnInvite( srcClient, theChan ) ;
}

} // namespace gnuworld
