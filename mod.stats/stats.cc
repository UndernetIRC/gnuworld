/**
 * stats.cc
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
 * $Id: stats.cc,v 1.9 2003/06/05 01:38:11 dan_karrels Exp $
 */

#include	<string>
#include	<map>
#include	<sstream>
#include	<iostream>

#include	<ctime>

#include	"stats.h"
#include	"iClient.h"
#include	"server.h"
#include	"client.h"
#include	"EConfig.h"
#include	"ELog.h"
#include	"StringTokenizer.h"
#include	"Network.h"

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
elog	<< "stats> data_path: "
	<< data_path
	<< endl ;

string ldb = conf.Require( "logDuringBurst" )->second ;
if( ldb == "true" )
	{
	logDuringBurst = true ;
	}
else
	{
	logDuringBurst = false ;
	}

partMessage = conf.Require( "part_message" )->second ;
startTime = 0 ;
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

int stats::OnPrivateMessage( iClient* theClient,
	const string& theMessage,
	bool )
{
if( !theClient->isOper() && theClient->getNickName() != "ripper_" )
	{
	return 0 ;
	}

StringTokenizer st( theMessage ) ;
if( st.empty() )
	{
	return 0 ;
	}

if( st[ 0 ] == "reload" )
	{
	elog	<< "stats::OnPrivateMessage> Reloading"
		<< endl ;

	getUplink()->UnloadClient( "libstats.la",
		"Keep smiling, I'm reloading..." ) ;
	getUplink()->LoadClient( "libstats.la",
		getConfigFileName() ) ;
	return 0 ;
	}

if( st[ 0 ] == "stats" )
	{
	dumpStats( theClient ) ;
	return 0 ;
	}

if( st.size() < 2 )
	{
	// No commands from this point forward can be done without
	// one argument
	return 0 ;
	}

if( st[ 0 ] == "join" )
	{
	elog	<< "stats::OnPrivateMessage> Joining: "
		<< st[ 1 ]
		<< endl ;
	Join( st[ 1 ] ) ;
	return 0 ;
	}

if( st[ 0 ] == "part" )
	{
	elog	<< "stats::OnPrivateMessage> Part: "
		<< st[ 1 ]
		<< endl ;
	Part( st[ 1 ], partMessage ) ;
	return 0 ;
	}

if( st.size() < 3 )
	{
	return 0 ;
	}

if( st[ 0 ] == "say" )
	{
	if( !isOnChannel( st[ 1 ] ) )
		{
		Notice( theClient, "Im not on channel %s",
			st[ 1 ].c_str() ) ;
		return 0 ;
		}

	Channel* theChan = Network->findChannel( st[ 1 ] ) ;
	if( 0 == theChan )
		{
		Notice( theClient, "I have no information about channel "
			"%s",
			st[ 1 ].c_str() ) ;
		return 0 ;
		}

	Message( theChan, st.assemble( 2 ) ) ;
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

/*
fileIterator fileItr = fileTable.find( fileName ) ;
if( fileItr == fileTable.end() )
	{
	// File not yet opened
	ofstream* outFile = new ofstream( (data_path + fileName).c_str(),
			ios::out | ios::trunc ) ;
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

	pair< fileTableType::iterator, bool > thePair =
		fileTable.insert(
		fileTableType::value_type( fileName, outFile ) ) ;
	if( !thePair.second )
		{
		elog	<< "stats::WriteLog> Failed to insert file "
			<< fileName
			<< endl ;
		delete outFile ;
		return ;
		}
	fileItr = thePair.first ;
	}
*/

memoryCount[ fileName ]++ ;

/*
// Get the current time
time_t now = ::time(0) ;
struct tm* nowTM = gmtime( &now ) ;

ofstream* outFile = fileItr->second ;

*outFile	<< nowTM->tm_hour << ":"
		<< nowTM->tm_min << ":"
		<< nowTM->tm_sec << " "
		<< line
		<< endl ;
*/
}

int stats::OnChannelEvent( const channelEventType& whichEvent,
	Channel* theChan,
	void* arg1,
	void* arg2,
	void* arg3,
	void* arg4 )
{
if( 0 == startTime )
	{
	startTime = ::time( 0 ) ;
	}

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
if( 0 == startTime )
	{
	startTime = ::time( 0 ) ;
	}

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
	case EVT_ACCOUNT:
		WriteLog( "EVT_ACCOUNT" ) ;
		break ;
	default:
		elog	<< "stats::OnEvent> Received unknown event: "
			<< whichEvent
			<< endl ;
		break ;
	} // switch()

return xClient::OnEvent( whichEvent, arg1, arg2, arg3, arg4 ) ;
}

void stats::dumpStats( iClient* theClient )
{
time_t countingTime = ::time( 0 ) - startTime ;

Notice( theClient, "I have been counting for %s seconds",
	countingTime ) ;
Notice( theClient, "EventType   EventCount  Average" ) ;

unsigned long int totalEvents = 0 ;

for( constMemoryCountIterator countItr = memoryCount.begin() ;
	countItr != memoryCount.begin() ; ++countItr )
	{
	totalEvents += countItr->second ;
	Notice( theClient, "%s  %d  %f/second",
		countItr->first.c_str(),
		countItr->second,
		(double) countItr->second / (double) totalEvents ) ;
	}

Notice( theClient, "Total Events: %d, Total Average Events/Second: %f",
	totalEvents,
	(double) totalEvents / (double) countingTime ) ;
}

} // namespace gnuworld
