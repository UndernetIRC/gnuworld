/* ccontrol.cc
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<string>
#include	<vector>
#include	<iostream>
#include	<algorithm>

#include	<cstring>

#include	"client.h"
#include	"iClient.h"
#include	"EConfig.h"
#include	"events.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"Network.h"
#include	"ELog.h"

#include	"ccontrol.h"
 
const char CControl_h_rcsId[] = __CCONTROL_H ;
const char CControl_cc_rcsId[] = "$Id: ccontrol.cc,v 1.2 2000/07/11 19:31:56 dan_karrels Exp $" ;

using std::string ;
using std::vector ;
using std::cout ;
using std::endl ; 

namespace gnuworld
{
 
ccontrol::ccontrol( const string& configFileName )
{
EConfig conf( configFileName ) ;

nickName = conf.Find( "nickname" )->second ;
userName = conf.Find( "username" )->second ;
hostName = conf.Find( "hostname" )->second ;
userDescription = conf.Find( "userdescription" )->second ;

Mode( conf.Find( "mode" )->second ) ;

operChanReason = conf.Find( "operchanreason" )->second ;
operChanModes = conf.Find( "operchanmodes" )->second ;
gLength = atoi( conf.Find( "glength" )->second.c_str() ) ;

// Set up the oper channels
EConfig::const_iterator ptr = conf.Find( "operchan" ) ;
while( ptr != conf.end() && ptr->first == "operchan" )
	{
	operChans.push_back( ptr->second ) ;
	++ptr ;
	}

for( ptr = conf.Find( "channel" ) ; ptr != conf.end() && ptr->first == "channel" ;
	++ptr )
	{
	StringTokenizer st( ptr->second ) ;
	if( st.size() > 1 )
		{
		channels.push_back( new e3Channel( st[ 0 ], st[ 1 ] ) ) ;
		}
	else
		{
		channels.push_back( new e3Channel( st[ 0 ] ) ) ;
		}
	} // for()

msgChan = conf.Find( "msgchan" )->second ;

// Make sure that the msgChan is in the list of operchans
if( operChans.end() == find( operChans.begin(), operChans.end(), msgChan ) )
	{
	operChans.push_back( msgChan ) ;
	}

// Be sure to use all capital letters for the command name
RegisterCommand( new HELPCommand( this, "HELP", "[topic]"
	"\t\tObtain general help or help for a specific command" ) ) ;
RegisterCommand( new INVITECommand( this, "INVITE", "<#channel>"
	"\t\tRequest an invitation to a channel" ) ) ;
RegisterCommand( new JUPECommand( this, "JUPE", "<servername> <reason>"
	"\tJupe a server for the given reason." ) ) ;
RegisterCommand( new MODECommand( this, "MODE", "<channel> <modes>"
	"\tChange modes on the given channel" ) ) ;
RegisterCommand( new GLINECommand( this, "GLINE", "[duration (sec)] <user@host> <reason>"
	"\tGline a given user@host for the given reason" ) ) ;
RegisterCommand( new SCANGLINECommand( this, "SCANGLINE", "<mask>"
	"\tSearch current network glines for glines matching <mask>" ) ) ;
RegisterCommand( new REMGLINECommand( this, "REMGLINE", "<user@host>"
	"\tRemove the gline matching <mask>" ) ) ;
RegisterCommand( new TRANSLATECommand( this, "TRANSLATE", "<numeric>"
	"\tTranslate a numeric into user information" ) ) ;
RegisterCommand( new WHOISCommand( this, "WHOIS", "<nickname>"
	"\tObtain information on a given nickname" ) ) ;
RegisterCommand( new KICKCommand( this, "KICK", "<channel> <nick>"
	"\tKick a user from a channel" ) ) ;
RegisterCommand( new ADDOPERCHANCommand( this, "ADDOPERCHAN", "<channel>"
	"\tAdd an oper channel" ) ) ;
RegisterCommand( new REMOPERCHANCommand( this, "REMOPERCHAN", "<channel>"
	"\tRemove an oper channel" ) ) ;
RegisterCommand( new LISTOPERCHANSCommand( this, "LISTOPERCHANS",
	"\tList current IRCoperator only channels" ) ) ;
RegisterCommand( new ADDCHANCommand( this, "ADDCHAN", "<channel>"
	"\tAdd a channel to be monitored" ) ) ;
RegisterCommand( new REMCHANCommand( this, "REMCHAN", "<channel>"
	"\tRemove a channel from being monitored" ) ) ;
RegisterCommand( new CHANINFOCommand( this, "CHANINFO", "<channel>"
	"\tObtain information about a given channel" ) ) ;

}

ccontrol::~ccontrol()
{
for( commandMapType::iterator ptr = commandMap.begin() ; ptr != commandMap.end() ; ++ptr )
	{
	delete ptr->second ;
	ptr->second = 0 ;
	}
commandMap.clear() ;
for( vector< e3Channel* >::iterator ptr = channels.begin() ; ptr != channels.end() ;
	++ptr )
	{
	delete *ptr ;
	*ptr = 0 ;
	}
channels.clear() ;
}

bool ccontrol::RegisterCommand( Command* newComm )
{
#ifdef EDEBUG
  assert( newComm != NULL ) ;
#endif

UnRegisterCommand( newComm->getName() ) ;
return commandMap.insert( pairType( newComm->getName(), newComm ) ).second ;
}

bool ccontrol::UnRegisterCommand( const string& commName )
{
commandMapType::iterator ptr = commandMap.find( commName ) ;
if( ptr == commandMap.end() )
	{
	return false ;
	}
delete ptr->second ;
commandMap.erase( ptr ) ;
return true ;
}

int ccontrol::BurstChannels()
{
//elog << "ccontrol::BurstChannels()\n" ;

// msgChan is an operChan as well, no need to burst it separately
for( vector< string >::size_type i = 0 ; i < operChans.size() ; i++ )
	{
	// Burst our channels
	MyUplink->JoinChannel( this, operChans[ i ], operChanModes ) ;

	// Receive events for this channel
	MyUplink->RegisterChannelEvent( operChans[ i ], this ) ;
	}

for( vector< e3Channel* >::size_type i = 0 ; i < channels.size() ; i++ )
	{
	// Burst our channels
	MyUplink->JoinChannel( this, channels[ i ]->chanName,
		channels[ i ]->modes ) ;

	// Receive events for this channel
	MyUplink->RegisterChannelEvent( channels[ i ]->chanName, this ) ;
	}

return xClient::BurstChannels() ;

}

// I don't really like doing this.
// In order for each of this bot's Command's to have a valid server
// pointer, this method must be overloaded and server must be
// explicitly set for each Command.
void ccontrol::ImplementServer( xServer* theServer )
{
for( commandMapType::iterator ptr = commandMap.begin() ; ptr != commandMap.end() ;
	++ptr )
	{
	ptr->second->setServer( theServer ) ;
	}
xClient::ImplementServer( theServer ) ;
}

int ccontrol::OnPrivateMessage( iClient* theClient, const string& Message )
{

//elog << "ccontrol::OnPrivateMessage()\n" ;

if( !theClient->isOper() && !theClient->getMode( iClient::MODE_SERVICES ) )
	{
	Notice( theClient, "You must be an IRCoperator to use this service." ) ;
	return 0 ;
	}

StringTokenizer st( Message ) ;
if( st.empty() )
	{
	Notice( theClient, "Incomplete command" ) ;
	return 0 ;
	}

const string Command = string_upper( st[ 0 ] ) ;

// Attempt to find a handler for this method.
commandMapType::iterator commHandler = commandMap.find( Command ) ;
if( commHandler == commandMap.end() )
	{
	Notice( theClient, "Unknown command" ) ;
	}
else
	{
	commHandler->second->Exec( theClient, Message ) ;
	}

return xClient::OnPrivateMessage( theClient, Message ) ;
}

int ccontrol::OnEvent( const eventType& theEvent,
	void* Data1, void* Data2, void* Data3, void* Data4 )
{
return xClient::OnEvent( theEvent, Data1, Data2, Data3, Data4 ) ;
}

int ccontrol::OnChannelEvent( const channelEventType& theEvent,
	const string& theChan,
	void* Data1, void* Data2, void* Data3, void* Data4 )
{

switch( theEvent )
	{
	case EVT_JOIN:
		iClient* theClient = static_cast< iClient* >( Data1 ) ;
		if( theClient->isOper() && (isOperChan( theChan ) 
			|| isRegularChan( theChan )) )
			{
			char buf[ 512 ] = { 0 } ;
			sprintf( buf, "%s M %s +o %s\n",
				getCharYYXXX().c_str(),
				theChan.c_str(),
				theClient->getCharYYXXX().c_str() ) ;
			QuoteAsServer( buf ) ;
			}
		break ;
	}

return xClient::OnChannelEvent( theEvent, theChan, Data1,
	Data2, Data3, Data4 ) ;
}

bool ccontrol::isRegularChan( const string& theChan ) const
{
vector< e3Channel* >::const_iterator ptr = channels.begin(),
				end = channels.end() ;
while( ptr != end )
	{
	if( !strcasecmp( (*ptr)->chanName.c_str(), theChan.c_str() ) )
		{
		return true ;
		}
	++ptr ;
	}
return false ;
}

bool ccontrol::isOperChan( const string& theChan ) const
{
vector< string >::const_iterator ptr = operChans.begin(),
	end = operChans.end() ;
while( ptr != end )
	{
	if( !strcasecmp( (*ptr).c_str(), theChan.c_str() ) )
		{
		return true ;
		}
	++ptr ;
	}
return false ;
}

// This method does NOT add the channel to any internal tables
bool ccontrol::Join( const string& chanName )
{
if( isOnChannel( chanName ) )
	{
	// Already on this channel
	return true ;
	}
bool result = xClient::Join( chanName ) ;
if( result )
	{
	MyUplink->RegisterChannelEvent( chanName, this ) ;
	}

return result ;
}

bool ccontrol::Part( const string& chanName )
{
bool foundChannel = false ;
for( vector< string >::iterator ptr = operChans.begin() ;
	ptr != operChans.end() ; ++ptr )
	{
	if( !strcasecmp( (*ptr).c_str(), chanName.c_str() ) )
		{
		operChans.erase( ptr ) ;
		foundChannel = true ;
		break ;
		}
	} // for()
for( vector< e3Channel* >::iterator ptr = channels.begin() ;
	ptr != channels.end() ; ++ptr )
	{
	if( NULL == *ptr )
		{
		elog	<< "ccontrol::Part> Found NULL!\n" ;
		continue ;
		}
	if( !strcasecmp( (*ptr)->chanName.c_str(), chanName.c_str() ) )
		{
		channels.erase( ptr ) ;
		delete *ptr ;
		foundChannel = true ;
		break ;
		}
	}

if( !foundChannel )
	{
	// The bot isn't on the channel
	return false ;
	}

bool result = xClient::Part( chanName ) ;
if( result )
	{
	MyUplink->UnRegisterChannelEvent( chanName, this ) ;
	}

return result ;
}

bool ccontrol::Kick( Channel* theChan, iClient* theClient, const string& reason )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

bool doPart = false ;
if( !isOnChannel( theChan->getName() ) )
	{
	// Im not on the channel
	doPart = true ;

	Join( theChan->getName() ) ;
	operChans.push_back( theChan->getName() ) ;
	}

bool result = xClient::Kick( theChan, theClient, reason ) ;

if( doPart )
	{
	Part( theChan->getName() ) ;
	}

return result ;
}

bool ccontrol::addOperChan( const string& chanName )
{
return addOperChan( chanName, operChanReason ) ;
}

bool ccontrol::addOperChan( const string& chanName, const string& reason )
{
if( isOperChan( chanName ) )
	{
	return false ;
	}

Join( chanName ) ;
operChans.push_back( chanName ) ;

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "ccontrol::addOperChan> Unable to find channel: "
		<< chanName << endl ;
	return false ;
	}

// Kick any users from the channel that aren't opers
vector< iClient* > clientsToKick ;
for( Channel::const_userIterator ptr = theChan->userList_begin() ;
	ptr != theChan->userList_end() ; ++ptr )
	{
	if( !ptr->second->isOper() )
		{
		clientsToKick.push_back( ptr->second->getClient() ) ;
		}
	}

if( !clientsToKick.empty() )
	{
	xClient::Kick( theChan, clientsToKick, reason ) ;
	}

// TODO: set operChanModes

return true ;
}

bool ccontrol::removeOperChan( const string& chanName )
{
// Part() will remove the channel from this client's tables.
Part( chanName ) ;

return true ;
}

bool ccontrol::addChan( const string& chanName )
{
if( isRegularChan( chanName ) )
	{
	return false ;
	}

Join( chanName ) ;
try
	{
	channels.push_back( new e3Channel( chanName ) ) ;
	}
catch( std::bad_alloc )
	{
	elog	<< "ccontrol::addChan> Memory allocation failure\n" ;
	return false ;
	}

return true ;
}

bool ccontrol::removeChan( const string& chanName )
{
// Part() will remove the given channel from the internal tables
Part( chanName ) ;

return true ;

} 

} // namespace gnuworld 
