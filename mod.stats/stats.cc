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
 * $Id: stats.cc,v 1.22 2003/08/09 23:15:36 dan_karrels Exp $
 */

#include	<string>
#include	<map>
#include	<sstream>
#include	<iostream>
#include	<iomanip>

#include	<ctime>

#include	"stats.h"
#include	"iClient.h"
#include	"server.h"
#include	"client.h"
#include	"EConfig.h"
#include	"ELog.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"config.h"
#include	"misc.h"

RCSTAG( "$Id: stats.cc,v 1.22 2003/08/09 23:15:36 dan_karrels Exp $" ) ;

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

allowOpers = false ;
string stringOperAccess = conf.Require( "allow_opers" )->second ;
if( !strcasecmp( stringOperAccess, "yes" ) ||
	!strcasecmp( stringOperAccess, "true" ) )
	{
	allowOpers = true ;
	}

EConfig::const_iterator ptr = conf.Find( "permit_user" ) ;
while( ptr != conf.end() && ptr->first == "permit_user" )
	{
	allowAccess.push_back( ptr->second ) ;
	++ptr ;
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
	if( EVT_RAW == whichEvent )
		{
		continue ;
		}
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
	if( EVT_RAW == whichEvent )
		{
		continue ;
		}

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

/*
void stats::ImplementServer( xServer* theServer )
{
xClient::ImplementServer( theServer ) ;

// Register for all events
for( eventType whichEvent = 0 ; whichEvent != EVT_NOOP ; ++whichEvent )
	{
	switch( whichEvent )
		{
		case EVT_RAW:
			break ;
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
*/

void stats::OnTimer( xServer::timerID, void* )
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
}

void stats::OnPrivateNotice( iClient* theClient,
	const string& theMessage,
	bool secure )
{
//elog	<< "stats::OnPrivateNotice> theClient: "
//	<< *theClient
//	<< ", theMessage: "
//	<< theMessage
//	<< endl ;
xClient::OnPrivateNotice( theClient,
	theMessage,
	secure ) ;
}

void stats::OnChannelNotice( iClient* theClient,
	Channel* theChan,
	const string& theMessage )
{
//elog	<< "stats::OnChannelNotice> theClient: "
//	<< *theClient
//	<< ", theChan: "
//	<< *theChan
//	<< ", theMessage: "
//	<< theMessage
//	<< endl ;
xClient::OnChannelNotice( theClient, theChan, theMessage ) ;
}

void stats::OnCTCP( iClient* theClient,
	const string& CTCPCommand,
	const string& theMessage,
	bool secure )
{
//elog	<< "stats::OnCTCP> theClient: "
//	<< *theClient
//	<< ", CTCPCommand: "
//	<< CTCPCommand
//	<< ", theMessage: "
//	<< theMessage
//	<< endl ;
xClient::OnCTCP( theClient, CTCPCommand,
	theMessage, secure ) ;
}

void stats::OnChannelMessage( iClient* theClient,
	Channel* theChan,
	const string& theMessage )
{
//elog	<< "stats::OnChannelMessage> theClient: "
//	<< *theClient
//	<< ", theChan: "
//	<< *theChan
//	<< ", theMessage: "
//	<< theMessage
//	<< endl ;
xClient::OnChannelMessage( theClient,
	theChan,
	theMessage ) ;
}

void stats::OnChannelCTCP( iClient* theClient,
	Channel* theChan,
	const string& CTCPCommand,
	const string& theMessage )
{
//elog	<< "stats::OnChannelCTCP> theClient: "
//	<< *theClient
//	<< ", theChan: "
//	<< *theChan
//	<< ", CTCPCommand: "
//	<< CTCPCommand
//	<< ", theMessage: "
//	<< theMessage
//	<< endl ;
xClient::OnChannelCTCP( theClient, theChan,
	CTCPCommand, theMessage ) ;
}

void stats::OnPrivateMessage( iClient* theClient,
	const string& theMessage,
	bool )
{
//elog	<< "stats::OnPrivateMessage> theClient: "
//	<< *theClient
//	<< ", theMessage: "
//	<< theMessage
//	<< endl ;

// Get rid of anyone who is not an oper and does not have access
bool userHasAccess = hasAccess( theClient->getAccount() ) ;
if( !userHasAccess && !theClient->isOper() )
	{
	// Normal user
	return ;
	}

if( !userHasAccess )
	{
	// The client must be an oper
	// Are opers allow to use the service?
	if( !allowOpers )
		{
		// Nope
		return ;
		}
	}

StringTokenizer st( theMessage ) ;
if( st.empty() )
	{
	return ;
	}

if( st[ 0 ] == "reload" )
	{
	elog	<< "stats::OnPrivateMessage> Reloading"
		<< endl ;

	getUplink()->UnloadClient( this,
		"Keep smiling, I'm reloading..." ) ;
	getUplink()->LoadClient( "libstats.la",
		getConfigFileName() ) ;
	return ;
	}

if( st[ 0 ] == "stats" )
	{
	dumpStats( theClient ) ;
	return ;
	}

if( st[ 0 ] == "shutdown" )
	{
	MyUplink->Shutdown() ;
	return ;
	}

if( st.size() < 2 )
	{
	// No commands from this point forward can be done without
	// one argument
	return ;
	}

if( st[ 0 ] == "join" )
	{
//	elog	<< "stats::OnPrivateMessage> Joining: "
//		<< st[ 1 ]
//		<< endl ;
	Join( st[ 1 ] ) ;
	return ;
	}

if( st[ 0 ] == "part" )
	{
//	elog	<< "stats::OnPrivateMessage> Part: "
//		<< st[ 1 ]
//		<< endl ;
	Part( st[ 1 ], partMessage ) ;
	return ;
	}

if( st.size() < 3 )
	{
	return ;
	}

if( st[ 0 ] == "say" )
	{
	if( !isOnChannel( st[ 1 ] ) )
		{
		Notice( theClient, "Im not on channel %s",
			st[ 1 ].c_str() ) ;
		return ;
		}

	Channel* theChan = Network->findChannel( st[ 1 ] ) ;
	if( 0 == theChan )
		{
		Notice( theClient, "I have no information about channel "
			"%s",
			st[ 1 ].c_str() ) ;
		return ;
		}

	Message( theChan, st.assemble( 2 ) ) ;
	return ;
	}
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
	if( EVT_RAW == whichEvent )
		{
		continue ;
		}

	ofstream& outFile = fileTable[ whichEvent ] ;

	outFile		<< nowTM->tm_hour << ":"
			<< nowTM->tm_min << ":"
			<< nowTM->tm_sec << " "
			<< eventMinuteTotal[ whichEvent ]
			<< endl ;
	}
}

void stats::OnChannelEvent( const channelEventType& whichEvent,
	Channel* theChan,
	void* arg1,
	void* arg2,
	void* arg3,
	void* arg4 )
{
if( !logDuringBurst && MyUplink->isBursting() )
	{
	// Don't log
	return ;
	}

if( 0 == startTime )
	{
	startTime = ::time( 0 ) ;
	}

assert( whichEvent <= EVT_CREATE ) ;

eventMinuteTotal[ whichEvent ]++ ;
eventTotal[ whichEvent ]++ ;

xClient::OnChannelEvent( whichEvent, theChan,
	arg1, arg2, arg3, arg4 ) ;
}

void stats::OnNetworkKick( Channel* theChan,
	iClient* srcClient,
	iClient* destClient,
	const string& kickMessage,
	bool authoritative )
{
eventMinuteTotal[ EVT_KICK ]++ ;
eventTotal[ EVT_KICK ]++ ;

xClient::OnNetworkKick( theChan,
	srcClient,
	destClient,
	kickMessage,
	authoritative ) ;
}

void stats::OnEvent( const eventType& whichEvent,
	void* arg1,
	void* arg2,
	void* arg3,
	void* arg4 )
{
if( !logDuringBurst && MyUplink->isBursting() )
	{
	// Don't log
	return ;
	}

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

xClient::OnEvent( whichEvent, arg1, arg2, arg3, arg4 ) ;
}

void stats::dumpStats( iClient* theClient )
{
time_t countingTime = ::time( 0 ) - startTime ;

Notice( theClient, "I have been counting for %d seconds",
	countingTime ) ;
Notice( theClient, "Total Network Users: %d, Total Network Channels: %d",
	Network->clientList_size(),
	Network->channelList_size() ) ;

Channel* largestChan = 0 ;
for( xNetwork::const_channelIterator chanItr = Network->channels_begin() ;
	chanItr != Network->channels_end() ; ++chanItr )
	{
	if( 0 == largestChan )
		{
		largestChan = chanItr->second ;
		}
	else if( chanItr->second->size() > largestChan->size() )
		{
		largestChan = chanItr->second ;
		}
	}

if( largestChan != 0 )
	{
	Notice( theClient, "Largest channel is %s, with %d users",
		largestChan->getName().c_str(),
		largestChan->size() ) ;
	}

#ifdef EDEBUG
	Notice( theClient, "Length of last burst: %d seconds, "
		"Number of bytes processed since beginning of last "
		"burst: %d",
		MyUplink->getLastBurstDuration(),
		MyUplink->getBurstBytes() ) ;
#endif

	{
	stringstream ss ;

	ss.width( 20 ) ;
	ss.setf( ios::left ) ;
	ss	<< "EventName" ;

	ss.width( 12 ) ;
	ss.setf( ios::left ) ;
	ss	<< "EventCount" ;

	ss.width( 15 ) ;
	ss.setf( ios::left ) ;
	ss	<< "Average" ;

	Notice( theClient, "%s", ss.str().c_str() ) ;
	}

unsigned long int totalEvents = 0 ;

// First, find the total number of events to occur
for( eventType whichEvent = 0 ; whichEvent <= EVT_CREATE ; ++whichEvent )
	{
	if( EVT_RAW == whichEvent )
		{
		continue ;
		}
	totalEvents += eventTotal[ whichEvent ] ;
	}

// Now output number of each event, and percentage of that
// event to the total events received
for( eventType whichEvent = 0 ; whichEvent <= EVT_CREATE ; ++whichEvent )
	{
	if( EVT_RAW == whichEvent )
		{
		continue ;
		}

	string writeMe ;
	stringstream ss ;

	ss.str( string() ) ;
	ss.setf( ios::left ) ;
	ss.fill( ' ' ) ;
//	ss.width( 25 ) ;
	ss	<< eventNames[ whichEvent ] ;
	writeMe = ss.str() ;

	ss.str( string() ) ;

	// For some reason, I can't get the stringstream IO
	// manipulation stuff to work properly here
	// *shrug* do it the hard way then...
	for( size_t i = 23 - eventNames[ whichEvent ].size() ; 
		i > 0 ; --i )
		{
		ss	<< ' ' ;
		}
	writeMe += ss.str() ;

	ss.str( string() ) ;
	ss.width( 12 ) ;
	ss.setf( ios::left ) ;
	ss	<< eventTotal[ whichEvent ] ;
	writeMe += ss.str() ;

	ss.str( string() ) ;
	ss.width( 15 ) ;
	ss.setf( ios::left ) ;
	ss	<< ((double) eventTotal[ whichEvent ] /
			(double) countingTime) ;
	writeMe += ss.str() ;

	Notice( theClient, "%s", writeMe.c_str() ) ;
	}

Notice( theClient, "Total Events: %d, Total Average Events/Second: %f",
	totalEvents,
	(double) totalEvents / (double) countingTime ) ;
}

bool stats::hasAccess( const string& accountName ) const
{
for( list< string >::const_iterator itr = allowAccess.begin() ;
	itr != allowAccess.end() ; ++itr )
	{
	if( !strcasecmp( accountName, *itr ) )
		{
		return true ;
		}
	}
return false ;
}

} // namespace gnuworld
