/**
 * stats.cc
 * Author: Daniel Karrels (dan@karrels.com)
 * $Id: stats.cc,v 1.5 2002/05/23 17:43:14 dan_karrels Exp $
 */

#include	<string>
#include	<map>
#include	<sstream>
#include	<iostream>

#include	"stats.h"
#include	"iClient.h"
#include	"server.h"
#include	"client.h"

namespace gnuworld
{

using std::string ;
using std::stringstream ;
using std::ends ;
using std::endl ;

/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */

extern "C"
{
  xClient* _gnuwinit(const string& args)
  {
    return new stats( args );
  }

}

stats::stats( const string& fileName )
 : xClient( fileName )
{}

stats::~stats()
{}

void stats::ImplementServer( xServer* theServer )
{
xClient::ImplementServer( theServer ) ;

for( eventType i = 0 ; i != EVT_NOOP ; ++i )
	{
	theServer->RegisterEvent( i, this ) ;
	}
}

int stats::OnPrivateMessage( iClient* theClient, const string& Message,
	bool )
{
if( !theClient->isOper() )
	{
	return 0 ;
	}

for( const_iterator ptr = table.begin(), end = table.end() ; ptr != end ;
	++ptr )
	{
	stringstream s ;
	s	<< ptr->first << ": " << ptr->second << ends ;

	Notice( theClient, s.str() ) ;
	} // for()

return 0 ;
}

int stats::OnEvent( const eventType& theEvent,
	void* data1, void* data2, void* data3, void* data4 )
{

//elog << "stats::OnEvent()" << endl ;

table[ "Total Events" ]++ ;

switch( theEvent )
	{
	case EVT_OPER:
		table[ "EVT_OPER" ]++ ;
		break ;
	case EVT_NETBREAK:
		table[ "EVT_NETBREAK" ]++ ;
		break ;
	case EVT_NETJOIN:
		table[ "EVT_NETJOIN" ]++ ;
		break ;
	case EVT_BURST_CMPLT:
		table[ "EVT_BURST_CMPLT" ]++ ;
		break ;
	case EVT_BURST_ACK:
		table[ "EVT_BURST_ACK" ]++ ;
		break ;
	case EVT_GLINE:
		table[ "EVT_GLINE" ]++ ;
		break ;
	case EVT_REMGLINE:
		table[ "EVT_REMGLINE" ]++ ;
		break ;
	case EVT_QUIT:
		table[ "EVT_QUIT" ]++ ;
		break ;
	case EVT_KILL:
		table[ "EVT_KILL" ]++ ;
		break ;
	case EVT_NICK:
		table[ "EVT_NICK" ]++ ;
		break ;
	default:
		table[ "UNKNOWN" ]++ ;
		break ;
	} // switch()

return 0 ;
}

} // namespace gnuworld
