/**
 * UnloadClientTimerHandler.h
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: UnloadClientTimerHandler.h,v 1.7 2004/05/19 19:46:33 jeekay Exp $
 */

#ifndef __UNLOADCLIENTTIMERHANDLER_H
#define __UNLOADCLIENTTIMERHANDLER_H "$Id: UnloadClientTimerHandler.h,v 1.7 2004/05/19 19:46:33 jeekay Exp $"

#include	<string>

#include	"ServerTimerHandlers.h"

namespace gnuworld
{

class xServer ;

/**
 * This class is responsible for unloading an xClient at a particular time.
 * This delayed unload allows the xClient to finish cleanup work as
 * requested by the server event system.
 */
class UnloadClientTimerHandler : public ServerTimerHandler
{

protected:

	/// The xClient module name
	std::string	moduleName ;

	/// The reason for the unload, will be delivered to the xClient
	std::string	reason ;

public:

	/**
	 * The constructor receives the follow arguments:
	 * - A pointer to the global xServer instance
	 * - The xClient module name
	 * - The reason for unloading the client.
	 */
	UnloadClientTimerHandler( xServer* theServer,
		const std::string& _moduleName,
		const std::string& _reason )
	: ServerTimerHandler( theServer, 0),
	  moduleName( _moduleName ),
	  reason( _reason )
	{}

	/**
	 * Destructor, not much to talk about here.
	 */
	virtual ~UnloadClientTimerHandler()
	{}

	/**
	 * The method that is called when it's time to unload the client.
	 */
	virtual	void	OnTimer( const timerID& , void* ) ;

} ;

} // namespace gnuworld

#endif // __UNLOADCLIENTTIMERHANDLER_H
