/* clientExample.h
 */

#ifndef __CLIENTEXAMPLE_H
#define __CLIENTEXAMPLE_H "$Id: clientExample.h,v 1.1 2001/01/06 15:31:53 dan_karrels Exp $"

#include	<string>

#include	"client.h"

using std::string ;

namespace gnuworld
{

/**
 * A dummy xClient.  This is just a skeleton from which developers
 * may create new services clients.
 */
class clientExample : public xClient
{

public:
	/**
	 * Constructor receives a configuration file name.
	 */
	clientExample( const string& ) ;

	/**
	 * Destructor does normal stuff.
	 */
	virtual ~clientExample() ;

	/**
	 * This method is called when a network client sends
	 * a private message (PRIVMSG or NOTICE) to this xClient.
	 * The first argument is a pointer to the source client,
	 * and the second argument is the actual message (minus
	 * all of the server command stuff).
	 */
	virtual int OnPrivateMessage( iClient*, const string& ) ;

	/**
	 * This method is called by the server when a server connection
	 * is established.  The purpose of this method is to inform
	 * the xServer of the channels this client wishes to burst.
	 */
	virtual int BurstChannels() ;

protected:

} ;

} // namespace gnuworld

#endif // __CLIENTEXAMPLE_H
