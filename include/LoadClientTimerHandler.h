/**
 * LoadClientTimerHandler.h
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
 * $Id: LoadClientTimerHandler.h,v 1.4 2003/06/28 16:26:45 dan_karrels Exp $
 */

#ifndef __LOADCLIENTTIMERHANDLER_H
#define __LOADCLIENTTIMERHANDLER_H "$Id: LoadClientTimerHandler.h,v 1.4 2003/06/28 16:26:45 dan_karrels Exp $"

#include	<string>

#include	"ServerTimerHandlers.h"

namespace gnuworld
{

using std::string ;

class xServer ;

/**
 * The purpose of this class is to load xClient's at a particular time,
 * it is used to perform xClient unload/loads while still allowing the
 * event system to give the xClient time to unload before reloading a
 * new instance.
 */
class LoadClientTimerHandler : public ServerTimerHandler
{

protected:

	/// The name of the module to be loaded
	string		moduleName ;

	/// The configuration file name to pass to the client constructor
	string		configFileName ;

public:

	/**
	 * Constructor receives:
	 * - The xServer instance for the system
	 * - The xClient's module name
	 * - The configuration file name for the xClient.
	 */
	LoadClientTimerHandler( xServer* theServer,
		 const string& _moduleName,
		 const string& _configFileName )
	: ServerTimerHandler( theServer, 0),
	  moduleName( _moduleName ),
	  configFileName( _configFileName )
	{}

	/**
	 * Default destructor, nothing to do here atm.
	 */
	virtual ~LoadClientTimerHandler()
	{}

	/**
	 * The method that is called by the server when this handler's
	 * time to perform has arrived.
	 */
	virtual	void	OnTimer( timerID, void* ) ;

} ;

} // namespace gnuworld

#endif // __LOADCLIENTTIMERHANDLER_H
