/**
 * stats.cc
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *                    Orlando Bassotto
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
 * $Id: stats.cc,v 1.6 2002/07/31 03:14:05 dan_karrels Exp $
 */

#include	<string>
#include	<map>
#include	<sstream>
#include	<iostream>

#include	"stats.h"
#include	"iClient.h"
#include	"server.h"
#include	"client.h"
#include	"EConfig.h"
#include	"ELog.h"

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
{
EConfig conf( fileName ) ;

data_path = conf.Require( "data_path" )->second ;
}

stats::~stats()
{
for( mapType::iterator streamItr = fileTable.begin() ;
	streamItr!= fileTable.end() ; ++streamItr )
	{
	(streamItr->second)->close() ;
	delete streamItr->second ;
	}
fileTable.clear() ;
}

void stats::ImplementServer( xServer* theServer )
{
xClient::ImplementServer( theServer ) ;

// Register for all events
for( eventType i = 0 ; i != EVT_NOOP ; ++i )
	{
	theServer->RegisterEvent( i, this ) ;
	}
theServer->RegisterChannelEvent( "*", this ) ;
}

int stats::OnPrivateMessage( iClient* theClient, const string& Message,
	bool )
{
if( !theClient->isOper() )
	{
	return 0 ;
	}

return 0 ;
}

void stats::WriteLog( const string& writeMe, const string& line )
{
mapType::iterator streamItr = fileTable.find( writeMe ) ;
if( streamItr == fileTable.end() )
	{
	// File not yet opened
	string fileName = data_path + writeMe ;

	ofstream* outFile = new ofstream( fileName.c_str() ) ;
	if( !(*outFile) )
		{
		elog	<< "stats::WriteLog> Unable to open file: "
			<< fileName
			<< endl ;
		return ;
		}

	streamItr = fileTable.insert(
		mapType::value_type( writeMe, outFile ) ).first ;
	}

// Get the current time
time_t now = ::time(0) ;
struct tm* nowTM = gmtime( &now ) ;

ofstream* outFile = streamItr->second ;

*outFile	<< nowTM->tm_hour << ":"
		<< nowTM->tm_min << ":"
		<< nowTM->tm_sec << " "
		<< line
		<< endl ;
}

int stats::OnChannelEvent( const channelEventType& whichEvent,
	Channel* theChan,
	void*, void*, void*, void* )
{


return 0 ;
}

int stats::OnEvent( const eventType& whichEvent,
	void* data1, void* data2, void* data3, void* data4 )
{
WriteLog( "Total_Events" ) ;

switch( whichEvent )
	{
	case EVT_OPER:
		WriteLog( "EVT_OPER" ) ;
		break ;
	case EVT_NETBREAK:
		WriteLog( "EVT_NETBREAK" ) ;
		break ;
	case EVT_NETJOIN:
		WriteLog( "EVT_NETJOIN" ) ;
		break ;
	case EVT_GLINE:
		WriteLog( "EVT_GLINE" ) ;
		break ;
	case EVT_REMGLINE:
		WriteLog( "EVT_REMGLINE" ) ;
		break ;
	case EVT_QUIT:
		WriteLog( "EVT_QUIT" ) ;
		break ;
	case EVT_KILL:
		WriteLog( "EVT_KILL" ) ;
		break ;
	case EVT_NICK:
		WriteLog( "EVT_NICK" ) ;
		break ;
	case EVT_RAW:
		{
		string* theLine = reinterpret_cast< string* >( data1 ) ;
		WriteLog( "EVT_RAW", *theLine ) ;
		}
		break ;
	case EVT_CHNICK:
		WriteLog( "EVT_CHNICK" ) ;
		break ;
	default:
		elog	<< "stats::OnEvent> Received unknown event: "
			<< whichEvent
			<< endl ;
		break ;
	} // switch()

return 0 ;
}

} // namespace gnuworld
