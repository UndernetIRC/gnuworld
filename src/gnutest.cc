/* gnutest.cc */

#include	<string>

#include	"client.h"
#include	"gnutest.h"
#include	"iClient.h"
#include	"StringTokenizer.h"
#include	"EConfig.h"

namespace gnuworld
{

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
	const string& chanName,
	void* data1, void* data2, void* data3, void* data4 )
{

if( chanName != operChan )
	{
	elog	<< "gnutest::OnChannelEvent> Got bad channel: "
		<< chanName << endl ;
	return 0 ;
	}

//elog	<< "gnutest::OnChannelEvent> Got channel: "
//	<< chanName << endl ;

iClient* theClient = 0 ;
Channel* theChan = 0 ;

switch( whichEvent )
	{
	case EVT_CREATE:
//		elog	<< "gnutest::OnChannelEvent> EVT_CREATE\n" ;
	case EVT_JOIN:
//		elog	<< "gnutest::OnChannelEvent> Got EVT_JOIN\n" ;
		theChan = static_cast< Channel* >( data1 ) ;
		theClient = static_cast< iClient* >( data2 ) ;

		if( theClient->isOper() )
			{
			Op( theChan, theClient ) ;
			}
		break ;
	default:
		break ;
	}

return xClient::OnChannelEvent( whichEvent, chanName, data1, data2,
	data3, data4 ) ;
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

if( st[ 0 ] == "invite" || st[ 0 ] == "INVITE" )
	{
	Invite( theClient, st[ 1 ] ) ;
	}
else if( st[ 0 ] == "moo" )
	{
	if( st.size() < 2 )
		{
		Notice( theClient, "Did you forget something?" ) ;
		return 0 ;
		}
	string raw = st.assemble( 1 ) ;
	Write( raw ) ;
	}

return xClient::OnPrivateMessage( theClient, message ) ;
}

} // namespace gnuworld
