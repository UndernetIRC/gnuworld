/* cloner.h
 * Author: Daniel Karrels dan@karrels.com
 */

#ifndef __CLONER_H
#define __CLONER_H "$Id: cloner.h,v 1.1 2001/01/12 22:49:24 dan_karrels Exp $"

#include	<string>
#include	<vector>

#include	<ctime>

#include	"client.h"
#include	"iClient.h"
#include	"iServer.h"

using std::string ;
using std::vector ;

namespace gnuworld
{

class cloner : public xClient
{

public:
	cloner( const string& configFileName ) ;
	virtual ~cloner() ;

	virtual int OnConnect() ;
	virtual int OnPrivateMessage( iClient*, const string&,
			bool secure = false ) ;
	virtual void addClone() ;

protected:

	virtual string		randomNick( int minLength = 3,
					int maxLength = 9 ) ;
	virtual string		randomUser() ;
	virtual string		randomHost() ;
	virtual string		randomMode() ;
	virtual char		randomChar() ;

	vector< iClient* >	clones ;
	vector< string >	userNames ;
	vector< string >	hostNames ;
	iServer*		fakeServer ;

	string			cloneDescription ;

} ;

} // namespace gnuworld

#endif // __CLONER_H
