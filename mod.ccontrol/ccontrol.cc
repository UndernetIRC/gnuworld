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
const char CControl_cc_rcsId[] = "$Id: ccontrol.cc,v 1.6 2001/01/28 15:49:05 dan_karrels Exp $" ;

using std::string ;
using std::vector ;
using std::cout ;
using std::endl ; 

namespace gnuworld
{

/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */

extern "C"
{
  xClient* _gnuwinit(const string& args)
  { 
    return new ccontrol( args );
  }

} 
 
ccontrol::ccontrol( const string& configFileName )
 : xClient( configFileName )
{

// Read the config file
EConfig conf( configFileName ) ;

// operChanReason is the reason used when kicking non-opers from
// oper-only channels
operChanReason = conf.Find( "operchanreason" )->second ;

// operChanModes are the modes to set when setting up an oper-only
// channel
operChanModes = conf.Find( "operchanmodes" )->second ;

// gLength is the length of time (in seconds) for default glines
gLength = atoi( conf.Find( "glength" )->second.c_str() ) ;

// Set up the oper channels
EConfig::const_iterator ptr = conf.Find( "operchan" ) ;
while( ptr != conf.end() && ptr->first == "operchan" )
	{
	operChans.push_back( ptr->second ) ;
	++ptr ;
	}

// Read out the client's message channel
msgChan = conf.Find( "msgchan" )->second ;

// Make sure that the msgChan is in the list of operchans
if( operChans.end() == find( operChans.begin(), operChans.end(), msgChan ) )
	{
	// Not found, add it to the list of operChans
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
RegisterCommand( new CHANINFOCommand( this, "CHANINFO", "<channel>"
	"\tObtain information about a given channel" ) ) ;

}

ccontrol::~ccontrol()
{
// Deallocate each command handler
for( commandMapType::iterator ptr = commandMap.begin() ;
	ptr != commandMap.end() ; ++ptr )
	{
	delete ptr->second ;
	ptr->second = 0 ;
	}
commandMap.clear() ;
}

// Register a command handler
bool ccontrol::RegisterCommand( Command* newComm )
{
#ifdef EDEBUG
  assert( newComm != NULL ) ;
#endif

// Unregister the command handler first; prevent memory leaks
UnRegisterCommand( newComm->getName() ) ;

// Insert the new handler
return commandMap.insert( pairType( newComm->getName(), newComm ) ).second ;
}

bool ccontrol::UnRegisterCommand( const string& commName )
{
// Find the command handler
commandMapType::iterator ptr = commandMap.find( commName ) ;

// Was the handler found?
if( ptr == commandMap.end() )
	{
	// Nope
	return false ;
	}

// Deallocate the handler
delete ptr->second ;

// Remove the handler
commandMap.erase( ptr ) ;

// Return success
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

// Don't forget to call the base class BurstChannels() method
return xClient::BurstChannels() ;

}

// I don't really like doing this.
// In order for each of this bot's Command's to have a valid server
// pointer, this method must be overloaded and server must be
// explicitly set for each Command.
void ccontrol::ImplementServer( xServer* theServer )
{
for( commandMapType::iterator ptr = commandMap.begin() ;
	ptr != commandMap.end() ; ++ptr )
	{
	ptr->second->setServer( theServer ) ;
	}
xClient::ImplementServer( theServer ) ;
}

int ccontrol::OnPrivateMessage( iClient* theClient, const string& Message,
	bool )
{

//elog << "ccontrol::OnPrivateMessage()\n" ;

// Only allow opers or services clients to use this client
if( !theClient->isOper() && !theClient->getMode( iClient::MODE_SERVICES ) )
	{
	Notice( theClient, "You must be an IRCoperator to use this service." ) ;
	return 0 ;
	}

// Tokenize the message
StringTokenizer st( Message ) ;

// Make sure there is a command present
if( st.empty() )
	{
	Notice( theClient, "Incomplete command" ) ;
	return 0 ;
	}

// This is no longer necessary, but oh well *shrug*
const string Command = string_upper( st[ 0 ] ) ;

// Attempt to find a handler for this method.
commandMapType::iterator commHandler = commandMap.find( Command ) ;

// Was a handler found?
if( commHandler == commandMap.end() )
	{
	// Nope, notify the client
	Notice( theClient, "Unknown command" ) ;
	}
else
	{
	// Yup, execute the handler
	commHandler->second->Exec( theClient, Message ) ;
	}

// Call the base class OnPrivateMessage() method
return xClient::OnPrivateMessage( theClient, Message ) ;
}

int ccontrol::OnEvent( const eventType& theEvent,
	void* Data1, void* Data2, void* Data3, void* Data4 )
{
return xClient::OnEvent( theEvent, Data1, Data2, Data3, Data4 ) ;
}

int ccontrol::OnChannelEvent( const channelEventType& theEvent,
	Channel* theChan,
	void* Data1, void* Data2, void* Data3, void* Data4 )
{

switch( theEvent )
	{
	case EVT_JOIN:
		if( !isOperChan( theChan ) )
			{
			// We really don't care otherwise
			// Note, this shouldn't happen
			break ;
			}

		iClient* theClient = static_cast< iClient* >( Data1 ) ;
		if( theClient->isOper() )
			{
			Op( theChan, theClient ) ;
			}
		break ;
	}

// Call the base class OnChannelEvent()
return xClient::OnChannelEvent( theEvent, theChan,
	Data1, Data2, Data3, Data4 ) ;
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

bool ccontrol::isOperChan( const Channel* theChan ) const
{
#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

return isOperChan( theChan->getName() ) ;
}

// This method does NOT add the channel to any internal tables
bool ccontrol::Join( const string& chanName )
{
if( isOnChannel( chanName ) )
	{
	// Already on this channel
	return true ;
	}
bool result = xClient::Join( chanName, string(), 0, true ) ;
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

bool ccontrol::Kick( Channel* theChan, iClient* theClient,
	const string& reason )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

if( !isOnChannel( theChan->getName() ) )
	{
	return false ;
	}

return xClient::Kick( theChan, theClient, reason ) ;
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

xClient::Join( chanName, operChanModes, 0, true ) ;
MyUplink->RegisterChannelEvent( chanName, this ) ;
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

} // namespace gnuworld 
