/**
 * scanner.h
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
 * $Id: scanner.h,v 1.2 2002/08/08 21:31:45 dan_karrels Exp $
 */

#ifndef __SCANNER_H
#define __SCANNER_H

#include	<string>

#include	"client.h"
#include	"server.h"
#include	"iClient.h"
#include	"dbThread.h"
#include	"ScannerModule.h"

using std::string ;

namespace gnuworld
{

/**
 * A scanner client for gnuworld.  It will attempt to determine if clients
 * connecting to the network have insecure proxies/socks/etc running.
 */
class scanner : public xClient
{

	dbThread		theThread ;

public:
	/**
	 * Constructor receives a configuration file name.
	 */
	scanner( const string& ) ;

	/**
	 * Destructor does normal stuff.
	 */
	virtual ~scanner() ;

	/**
	 * This method is called when a network client sends
	 * a private message (PRIVMSG or NOTICE) to this xClient.
	 * The first argument is a pointer to the source client,
	 * and the second argument is the actual message (minus
	 * all of the server command stuff).
	 */
	virtual int OnPrivateMessage( iClient*, const string&,
		bool secure = false ) ;

	/**
	 * Calls the base class ImplementServer() and registers for
	 * events.
	 */
	virtual void	ImplementServer( xServer* theServer ) ;

	/**
	 * This method is called by the server when a server connection
	 * is established.  The purpose of this method is to inform
	 * the xServer of the channels this client wishes to burst.
	 */
	virtual int BurstChannels() ;

	/**
	 * The event handler method, all network events will cause this
	 * method to be invoked.
	 */
	virtual int	OnEvent( const eventType&,
				void* = 0,
				void* = 0,
				void* = 0,
				void* = 0 ) ;

	/* For use by scanner modules */

	/**
	 * RejectClient() is called to notify the scanner that the
	 * given Connection represents an insecurity.
	 * If the optional second argument is specified, then it will
	 * be used in log messages and G-Line.
	 */
	virtual void	RejectClient( Connection*,
				const string& = string() ) ;

protected:

	/**
	 * This method is called when a new client connects to the network.
	 */
	virtual void	handleNewClient( iClient* ) ;

} ;

} // namespace gnuworld

#endif // __SCANNER_H
