#ifndef __TESTBOT_H
#define __TESTBOT_H "$Id: testbot.h,v 1.1 2003/06/03 01:01:56 dan_karrels Exp $"

#include	<string>
#include	<list>

#include	"ConnectionManager.h"
#include	"ConnectionHandler.h"
#include	"Connection.h"

using namespace gnuworld ;

using std::string ;
using std::list ;

class testBot : public ConnectionHandler
{

public:
	testBot( const string& ) ;
	virtual ~testBot() ;

	virtual void	OnConnect( Connection* ) ;
	virtual void	OnConnectFail( Connection* ) ;
	virtual void	OnRead( Connection*, const string& ) ;
	virtual void	OnDisconnect( Connection* ) ;
	virtual void	OnTimeout( Connection* ) ;

	virtual void	Run() ;

	inline const string&	getUplinkName() const
		{ return uplinkName ; }

	inline unsigned short int getUplinkPort() const
		{ return uplinkPort ; }

	inline const string&	getNickName() const
		{ return nickName ; }

	inline const string&	getUserName() const
		{ return userName ; }

	inline const string&	getRealName() const
		{ return realName ; }

	inline void	setUplinkName( const string& newVal )
		{ uplinkName = newVal ; }

	inline void	setUplinkPort( unsigned short int newVal )
		{ uplinkPort = newVal ; }

	inline void	setNickName( const string& newVal )
		{ nickName = newVal ; }

	inline void	setUserName( const string& newVal )
		{ userName = newVal ; }

	inline void	setRealName( const string& newVal )
		{ realName = newVal ; }

protected:

	virtual void	login() ;
	virtual void	joinChannels() ;
	virtual void	processInput( const string& ) ;

	bool			keepRunning ;

	string			uplinkName ;
	unsigned short int	uplinkPort ;
	string			nickName ;
	string			userName ;
	string			realName ;

	Connection*		myConnect ;
	ConnectionManager	cm ;

	list< string >		channels ;
} ;

#endif // __TESTBOT_H
