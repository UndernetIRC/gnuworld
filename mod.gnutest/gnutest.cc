/* gnutest.cc */

#include	<string>

#include	"client.h"
#include	"gnutest.h"
#include	"iClient.h"
#include	"StringTokenizer.h"
#include	"EConfig.h"
#include	"Network.h"

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
    return new gnutest( args );
  }

} 
 
using std::string ;

gnutest::gnutest( const string& fileName )
 : xClient( fileName )
{
EConfig conf( fileName ) ;

operChan = conf.Require( "operchan" )->second ;

}

gnutest::~gnutest()
{}

int gnutest::BurstChannels()
{
Join( operChan ) ;
MyUplink->RegisterChannelEvent( operChan, this ) ;
return xClient::BurstChannels() ;
}

int gnutest::OnChannelEvent( const channelEventType& whichEvent,
	Channel* theChan,
	void* data1, void* data2, void* data3, void* data4 )
{

if( theChan->getName() != operChan )
	{
	elog	<< "gnutest::OnChannelEvent> Got bad channel: "
		<< theChan->getName() << endl ;
	return 0 ;
	}

iClient* theClient = 0 ;

switch( whichEvent )
	{
	case EVT_CREATE:
//		elog	<< "gnutest::OnChannelEvent> EVT_CREATE\n" ;
	case EVT_JOIN:
//		elog	<< "gnutest::OnChannelEvent> Got EVT_JOIN\n" ;
		theClient = static_cast< iClient* >( data1 ) ;

		if( theClient->isOper() )
			{
			Op( theChan, theClient ) ;
			}
		break ;
	default:
		break ;
	}

return xClient::OnChannelEvent( whichEvent, theChan,
	data1, data2, data3, data4 ) ;
}

int gnutest::OnEvent( const eventType& whichEvent,
	void* data1, void* data2, void* data3, void* data4 )
{


return xClient::OnEvent( whichEvent, data1, data2, data3, data4 ) ;
}

int gnutest::OnPrivateMessage( iClient* theClient, const string& message )
{

if( !theClient->isOper() )
	{
	return 0 ;
	}

StringTokenizer st( message ) ;
if( st.empty() || (st.size() < 2 ) )
	{
	Notice( theClient, "Are you speaking to me?" ) ;
	return 0 ;
	}

if( st[ 0 ] == "invite" )
	{
	Invite( theClient, st[ 1 ] ) ;
	}
else if( st[ 0 ] == "moo" )
	{
	string raw = st.assemble( 1 ) ;
	Write( raw ) ;
	}
else if( st[ 0 ] == "join" )
	{
	Join( st[ 1 ] ) ;
	}
else if( st[ 0 ] == "part" )
	{
	Part( st[ 1 ] ) ;
	}
else if( st[ 0 ] == "ban" )
	{
	if( st.size() != 3 )
		{
		Notice( theClient, "Usage: ban #channel nickname" ) ;
		return 0 ;
		}

	Channel* theChan = Network->findChannel( st[ 1 ] ) ;
	if( NULL == theChan )
		{
		Notice( theClient, "Unable to find channel" ) ;
		return 0 ;
		}

	iClient* theClient = Network->findNick( st[ 2 ] ) ;
	if( NULL == theClient )
		{
		Notice( theClient, "Unable to find nickname" ) ;
		return 0 ;
		}

	if( 0 == theChan->findUser( theClient ) )
		{
		Notice( theClient, "The user doesn't appear to be on that channel" ) ;
		return 0 ;
		}

	Ban( theChan, theClient ) ;
	}
else if( st[ 0 ] == "unban" )
	{
	if( st.size() != 3 )
		{
		Notice( theClient, "Usage: unban #channel banmask" ) ;
		return 0 ;
		}

	Channel* theChan = Network->findChannel( st[ 1 ] ) ;
	if( NULL == theChan )
		{
		Notice( theClient, "Unable to find channel" ) ;
		return 0 ;
		}

	if( !theChan->findBan( st[ 2 ] ) )
		{
		Notice( theClient, "Unable to find ban" ) ;
		return 0 ;
		}

	UnBan( theChan, st[ 2 ] ) ;
	}
else if( st[ 0 ] == "bankick" )
	{
	if( st.size() < 4 )
		{
		Notice( theClient, "Usage: bankick #channel nick reason" ) ;
		return 0 ;
		}

	Channel* theChan = Network->findChannel( st[ 1 ] ) ;
	if( NULL == theChan )
		{
		Notice( theClient, "Unable to find channel" ) ;
		return 0 ;
		}

	iClient* theClient = Network->findNick( st[ 2 ] ) ;
	if( NULL == theClient )
		{
		Notice( theClient, "Unable to find nickname" ) ;
		return 0 ;
		}

	if( 0 == theChan->findUser( theClient ) )
		{
		Notice( theClient, "The user doesn't appear to be on that channel" ) ;
		return 0 ;
		}

	BanKick( theChan, theClient, st.assemble( 3 ) ) ;
	}
else if( st[ 0 ] == "op" )
	{
	if( st.size() != 3 )
		{
		Notice( theClient, "Usage: op #channel nick" ) ;
		return 0 ;
		}

	Channel* theChan = Network->findChannel( st[ 1 ] ) ;
	if( NULL == theChan )
		{
		Notice( theClient, "Unable to find channel" ) ;
		return 0 ;
		}

	iClient* theClient = Network->findNick( st[ 2 ] ) ;
	if( NULL == theClient )
		{
		Notice( theClient, "Unable to find nickname" ) ;
		return 0 ;
		}

	if( 0 == theChan->findUser( theClient ) )
		{
		Notice( theClient, "The user doesn't appear to be on that channel" ) ;
		return 0 ;
		}

	Op( theChan, theClient ) ;
	}
else if( st[ 0 ] == "deop" )
	{
	if( st.size() != 3 )
		{
		Notice( theClient, "Usage: deop #channel nick" ) ;
		return 0 ;
		}

	Channel* theChan = Network->findChannel( st[ 1 ] ) ;
	if( NULL == theChan )
		{
		Notice( theClient, "Unable to find channel" ) ;
		return 0 ;
		}

	iClient* theClient = Network->findNick( st[ 2 ] ) ;
	if( NULL == theClient )
		{
		Notice( theClient, "Unable to find nickname" ) ;
		return 0 ;
		}

	if( 0 == theChan->findUser( theClient ) )
		{
		Notice( theClient, "The user doesn't appear to be on that channel" ) ;
		return 0 ;
		}

	DeOp( theChan, theClient ) ;
	}
else if( st[ 0 ] == "schedule" )
	{
	Channel* theChan = Network->findChannel( st[ 1 ] ) ;
	if( NULL == theChan )
		{
		Notice( theClient, "Unable to find channel" ) ;
		return 0 ;
		}

	xServer::timerID id = MyUplink->RegisterTimer( ::time( 0 ) + 60,
		this ) ;
	if( 0 == id )
		{
		Notice( theClient, "Failed" ) ;
		}
	else
		{
		Notice( theClient, "Scheduled for 1 minute from now" ) ;
		timerChan = theChan->getName() ;
		}
	}

return xClient::OnPrivateMessage( theClient, message ) ;
}

bool gnutest::isOnChannel( const string& chanName ) const
{
if( !::strcasecmp( chanName.c_str(), operChan.c_str() ) )
	{
	return true ;
	}

return std::find( channels.begin(), channels.end(), chanName )
	!= channels.end() ;
}

bool gnutest::isOnChannel( const Channel* theChan ) const
{
#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

return isOnChannel( theChan->getName() ) ;
}

bool gnutest::addChan( Channel* theChan )
{
#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

if( isOnChannel( theChan->getName() ) )
	{
	// Already on the channel
	return false ;
	}

channels.push_back( theChan->getName() ) ;

return true ;
}

bool gnutest::removeChan( Channel* theChan )
{
#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

std::remove( channels.begin(), channels.end(), theChan->getName() ) ;

return true ;
}

int gnutest::OnTimer( xServer::timerID id, void* data )
{
Channel* theChan = Network->findChannel( timerChan ) ;
if( NULL == theChan )
	{
	elog	<< "gnutest::OnTimer> Unable to find channel: "
		<< timerChan << endl ;
	return 0 ;
	}

Message( theChan, "Respect my authoritah!" ) ;

return 0 ;
}

} // namespace gnuworld
