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
 * $Id: stats.cc,v 1.12 2003/06/06 20:03:31 dan_karrels Exp $
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

openLogFiles() ;

memset( eventMinuteTotal, 0, sizeof( eventMinuteTotal ) ) ;
memset( eventTotal, 0, sizeof( eventTotal ) ) ;
}

stats::~stats()
{
for( eventType whichEvent = 0 ; whichEvent <= EVT_CREATE ; ++whichEvent )
	{
	fileTable[ whichEvent ].flush() ;
	fileTable[ whichEvent ].close() ;
	}
}

void stats::openLogFiles()
{
// For each possible event, open a log file to receive minutely
// event totals.
// Note that the file names will be retrieve from the eventNames
// array, and spaces (' ') will be substituted with underscore ('_').
for( eventType whichEvent = 0 ; whichEvent <= EVT_CREATE ; ++whichEvent )
	{
	string fileName( eventNames[ whichEvent ] ) ;

	// Substitute ' ' for '_'
	for( string::iterator sItr = fileName.begin() ;
		sItr != fileName.end() ; ++sItr )
		{
		if( ' ' == *sItr )
			{
			*sItr = '_' ;
			}
		} // for( sItr )

	// Update the fileName to include the full path
	fileName = data_path + fileName ;

	// File not yet opened
	fileTable[ whichEvent ].open( fileName.c_str(),
		ios::out | ios::trunc ) ;
	if( !fileTable[ whichEvent ].is_open() )
		{
		elog	<< "stats::openLogFiles> Unable to open file: "
			<< fileName
			<< endl ;
		return ;
		}

//	elog	<< "stats::openLogFiles> Opened log file: "
//		<< fileName
//		<< endl ;
	} // for( whichEvent )
}

void stats::ImplementServer( xServer* theServer )
{
xClient::ImplementServer( theServer ) ;

// Register for all events
for( eventType whichEvent = 0 ; whichEvent != EVT_NOOP ; ++whichEvent )
	{
	switch( whichEvent )
		{
		default:
			theServer->RegisterEvent( whichEvent, this ) ;
			break ;
		} // switch()
	} // for()

theServer->RegisterChannelEvent( "*", this ) ;

// Register to receive timed events every minute
// This event will be used to flush data to the log files
theServer->RegisterTimer( ::time( 0 ) + 60, this ) ;
}

int stats::OnTimer( xServer::timerID, void* )
{
//elog	<< "stats::OnTimer"
//	<< endl ;

// Timed events are once-run, make sure to request a new
// timed event 1 minute from now
MyUplink->RegisterTimer( ::time( 0 ) + 60, this ) ;

// Flush logs
writeLog() ;

// Reset the minutely event counters


memset( eventMinuteTotal, 0, sizeof( eventMinuteTotal ) ) ;

return 0 ;
}

int stats::OnChannelMessage( iClient* theClient,
	Channel* theChan,
	const string& theMessage )
{
elog	<< "stats::OnChannelMessage> theClient: "
	<< *theClient
	<< ", theChan: "
	<< *theChan
	<< ", theMessage: "
	<< theMessage
	<< endl ;
return xClient::OnChannelMessage( theClient,
	theChan,
	theMessage ) ;
}

int stats::OnPrivateMessage( iClient* theClient,
	const string& theMessage,
	bool )
{
elog	<< "stats::OnPrivateMessage> theClient: "
	<< *theClient
	<< ", theMessage: "
	<< theMessage
	<< endl ;

if( !theClient->isOper() &&
	((theClient->getMode( iClient::MODE_REGISTERED )) &&
	(theClient->getAccount() != "reppir")) )
	{
	elog	<< "stats::OnPrivateMessage> Denying access"
		<< endl ;
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

	getUplink()->UnloadClient( this,
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
//	elog	<< "stats::OnPrivateMessage> Joining: "
//		<< st[ 1 ]
//		<< endl ;
	Join( st[ 1 ] ) ;
	return 0 ;
	}

if( st[ 0 ] == "part" )
	{
//	elog	<< "stats::OnPrivateMessage> Part: "
//		<< st[ 1 ]
//		<< endl ;
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

void stats::writeLog()
{
// Should we log during a net burst
if( !logDuringBurst && MyUplink->isBursting() )
	{
	// Don't log
	return ;
	}

// Get the current time
time_t now = ::time(0) ;
struct tm* nowTM = gmtime( &now ) ;

for( eventType whichEvent = 0 ; whichEvent <= EVT_CREATE ; ++whichEvent )
	{
	ofstream& outFile = fileTable[ whichEvent ] ;

	outFile		<< nowTM->tm_hour << ":"
			<< nowTM->tm_min << ":"
			<< nowTM->tm_sec << " "
			<< eventMinuteTotal[ whichEvent ]
			<< endl ;
	}
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

assert( whichEvent <= EVT_CREATE ) ;

eventMinuteTotal[ whichEvent ]++ ;
eventTotal[ whichEvent ]++ ;

return xClient::OnChannelEvent( whichEvent, theChan,
	arg1, arg2, arg3, arg4 ) ;
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

assert( whichEvent <= EVT_CREATE ) ;

eventMinuteTotal[ whichEvent ]++ ;
eventTotal[ whichEvent ]++ ;

// NEVER uncomment this line on a large network heh
//elog	<< "stats::OnEvent> Event number: "
//	<< whichEvent
//	<< endl ;

return xClient::OnEvent( whichEvent, arg1, arg2, arg3, arg4 ) ;
}

void stats::dumpStats( iClient* theClient )
{
time_t countingTime = ::time( 0 ) - startTime ;

Notice( theClient, "I have been counting for %d seconds",
	countingTime ) ;
Notice( theClient, "EventName   EventCount  Average" ) ;

unsigned long int totalEvents = 0 ;

// First, find the total number of events to occur
for( eventType whichEvent = 0 ; whichEvent <= EVT_CREATE ; ++whichEvent )
	{
	totalEvents += eventTotal[ whichEvent ] ;
	}

// Now output number of each event, and percentage of that
// event to the total events received
for( eventType whichEvent = 0 ; whichEvent <= EVT_CREATE ; ++whichEvent )
	{
	Notice( theClient, "%s  %d  %f/second",
		eventNames[ whichEvent ].c_str(),
		eventTotal[ whichEvent ],
		(double) eventTotal[ whichEvent ] /
			(double) totalEvents ) ;
	}

Notice( theClient, "Total Events: %d, Total Average Events/Second: %f",
	totalEvents,
	(double) totalEvents / (double) countingTime ) ;
}

} // namespace gnuworld
