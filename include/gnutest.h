/* gnutest.h */

#ifndef __GNUTEST_H
#define __GNUTEST_H "$Id: gnutest.h,v 1.1 2000/08/01 00:03:23 dan_karrels Exp $"

#include	<string>

#include	"client.h"
#include	"iClient.h"

namespace gnuworld
{

class gnutest : public xClient
{

public:
	gnutest( const string& ) ;
	virtual ~gnutest() ;

	virtual int OnChannelEvent( const channelEventType&,
		const string&,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	virtual int OnEvent( const eventType& theEvent,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	virtual int BurstChannels() ;
	virtual int OnPrivateMessage( iClient*, const string& ) ;

	virtual bool isOnChannel( const string& chanName ) const
		{ return (chanName == operChan) ; }
	virtual bool isOnChannel( const Channel* theChan ) const
		{ return (theChan->getName() == operChan) ; }

protected:

	string		operChan ;

} ;

} // namespace gnuworld

#endif // __GNUTEST_H
