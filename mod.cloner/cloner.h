/**
 * cloner.h
 * Load fake clones for testing or fun.
 *
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *		      Reed Loden <reed@reedloden.com>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * $Id: cloner.h,v 1.10 2004/06/04 20:17:23 jeekay Exp $
 */

#ifndef __CLONER_H
#define __CLONER_H "$Id: cloner.h,v 1.10 2004/06/04 20:17:23 jeekay Exp $"

#include	<string>
#include	<vector>
#include	<list>

#include	<ctime>

#include	"client.h"
#include	"iClient.h"
#include	"iServer.h"

namespace gnuworld
{

class cloner : public xClient
{

public:
	cloner( const std::string& configFileName ) ;
	virtual ~cloner() ;

	virtual void OnConnect() ;
	virtual void OnTimer( const xServer::timerID&, void* ) ;
	virtual void OnPrivateMessage( iClient*, const std::string&,
			bool secure = false ) ;
	virtual void addClone() ;

protected:

	virtual bool		hasAccess( const std::string& ) const ;

	virtual std::string	randomNick( int minLength = 5,
					int maxLength = 9 ) ;
	virtual std::string	randomUser() ;
	virtual std::string	randomHost() ;
	virtual char		randomChar() ;

	std::list< std::string >	allowAccess ;
	std::list< iClient* >		clones ;
	std::vector< std::string >	userNames ;
	std::vector< std::string >	hostNames ;
	iServer*		fakeServer ;

	bool			allowOpers ;

	size_t			makeCloneCount ;
	size_t			cloneBurstCount ;
	size_t			minNickLength ;
	size_t			maxNickLength ;

	std::string		cloneDescription ;
	std::string		cloneMode ;
	std::string		fakeServerName ;
	std::string		fakeServerDescription ;

} ;

} // namespace gnuworld

#endif // __CLONER_H
