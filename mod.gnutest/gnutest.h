/* gnutest.h */

#ifndef __GNUTEST_H
#define __GNUTEST_H "$Id: gnutest.h,v 1.1 2000/12/13 23:22:23 dan_karrels Exp $"

#include	<string>
#include	<vector>

#include	"client.h"
#include	"iClient.h"
#include	"Channel.h"

namespace gnuworld
{

using std::string ;
using std::vector ;

class gnutest : public xClient
{

public:
	gnutest( const string& ) ;
	virtual ~gnutest() ;

	virtual int	OnChannelEvent( const channelEventType&,
		Channel*,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	virtual int	OnEvent( const eventType& theEvent,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	virtual int	BurstChannels() ;
	virtual int	OnPrivateMessage( iClient*, const string& ) ;
	virtual int	OnTimer( xServer::timerID, void* ) ;

	virtual bool	isOnChannel( const string& chanName ) const ;
	virtual bool	isOnChannel( const Channel* theChan ) const ;

protected:

	virtual bool	addChan( Channel* ) ;
	virtual bool	removeChan( Channel* ) ;

	string		operChan ;
	string		timerChan ;
	vector< string >	channels ;

} ;

} // namespace gnuworld

#endif // __GNUTEST_H
