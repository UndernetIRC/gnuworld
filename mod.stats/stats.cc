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
 * $Id: stats.cc,v 1.8 2002/08/08 18:50:05 dan_karrels Exp $
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

using std::cerr ;
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
if( data_path.empty() )
	{
	cerr	<< "stats> Invalid data_path"
		<< endl ;
	::exit( -1 ) ;
	}

// Make sure that the last char is a '/'
if( '/' != data_path[ data_path.size() - 1 ] )
	{
	data_path += "/" ;
	}

string ldb = conf.Require( "logDuringBurst" )->second ;
if( ldb == "true" )
	{
	logDuringBurst = true ;
	}
else
	{
	logDuringBurst = false ;
	}

elog	<< "stats> data_path: "
	<< data_path
	<< endl ;
}

stats::~stats()
{
for( fileIterator fileItr = fileTable.begin() ;
	fileItr!= fileTable.end() ; ++fileItr )
	{
	(fileItr->second)->flush() ;
	(fileItr->second)->close() ;
	delete fileItr->second ;
	}
fileTable.clear() ;
}

void stats::ImplementServer( xServer* theServer )
{
xClient::ImplementServer( theServer ) ;

// Register for all events
for( eventType whichEvent = 0 ; whichEvent != EVT_NOOP ; ++whichEvent )
	{
	switch( whichEvent )
		{
		case EVT_RAW:
		case EVT_BURST_CMPLT:
		case EVT_BURST_ACK:
			break ;
		default:
			theServer->RegisterEvent( whichEvent, this ) ;
			break ;
		} // switch()
	} // for()

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

void stats::WriteLog( const string& fileName, const string& line )
{
//elog	<< "stats::WriteLog> fileName: "
//	<< fileName
//	<< ", line: "
//	<< line
//	<< endl ;

// Should we log during a net burst
if( !logDuringBurst && MyUplink->isBursting() )
	{
	// Don't log
	return ;
	}

fileIterator fileItr = fileTable.find( fileName ) ;
if( fileItr == fileTable.end() )
	{
	// File not yet opened
	ofstream* outFile = new ofstream( (data_path + fileName).c_str() ) ;
	if( !(*outFile) )
		{
		elog	<< "stats::WriteLog> Unable to open file: "
			<< (data_path + fileName)
			<< endl ;
		return ;
		}

	elog	<< "stats::writeLog> Opened log file: "
		<< (data_path + fileName)
		<< endl ;

	fileItr = fileTable.insert(
		fileTableType::value_type( fileName, outFile ) ).first ;
	}

// Get the current time
time_t now = ::time(0) ;
struct tm* nowTM = gmtime( &now ) ;

ofstream* outFile = fileItr->second ;

*outFile	<< nowTM->tm_hour << ":"
		<< nowTM->tm_min << ":"
		<< nowTM->tm_sec << " "
		<< line
		<< endl ;
}

int stats::OnChannelEvent( const channelEventType& whichEvent,
	Channel* theChan,
	void* arg1,
	void* arg2,
	void* arg3,
	void* arg4 )
{
switch( whichEvent )
	{
	case EVT_JOIN:
		WriteLog( "EVT_JOIN" ) ;
		break ;
	case EVT_PART:
		WriteLog( "EVT_PART" ) ;
		break ;
	case EVT_TOPIC:
		WriteLog( "EVT_TOPIC" ) ;
		break ;
	case EVT_KICK:
		WriteLog( "EVT_KICK" ) ;
		break ;
	case EVT_CREATE:
		WriteLog( "EVT_CREATE" ) ;
		break ;
	}

return xClient::OnChannelEvent( whichEvent, theChan, arg1, arg2, arg3, arg4 ) ;
}

int stats::OnEvent( const eventType& whichEvent,
	void* arg1,
	void* arg2,
	void* arg3,
	void* arg4 )
{
// NEVER uncomment this line on a large network heh
//elog	<< "stats::OnEvent> Event number: "
//	<< whichEvent
//	<< endl ;

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
	case EVT_CHNICK:
		WriteLog( "EVT_CHNICK" ) ;
		break ;
	default:
		elog	<< "stats::OnEvent> Received unknown event: "
			<< whichEvent
			<< endl ;
		break ;
	} // switch()

return xClient::OnEvent( whichEvent, arg1, arg2, arg3, arg4 ) ;
}

} // namespace gnuworld
