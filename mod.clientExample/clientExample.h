/**
 * clientExample.h
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
 * $Id: clientExample.h,v 1.6 2004/05/25 14:17:58 jeekay Exp $
 */

#ifndef __CLIENTEXAMPLE_H
#define __CLIENTEXAMPLE_H "$Id: clientExample.h,v 1.6 2004/05/25 14:17:58 jeekay Exp $"

#include	<string>

#include	"client.h"

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
	clientExample( const std::string& ) ;

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
	virtual void OnPrivateMessage( iClient*, const std::string&,
		bool secure = false ) ;

	/**
	 * This method is called by the server when a server connection
	 * is established.  The purpose of this method is to inform
	 * the xServer of the channels this client wishes to burst.
	 */
	virtual void BurstChannels() ;

protected:

} ;

} // namespace gnuworld

#endif // __CLIENTEXAMPLE_H
