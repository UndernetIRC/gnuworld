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
 * $Id: cloner.h,v 1.8 2003/07/21 23:36:34 dan_karrels Exp $
 */

#ifndef __CLONER_H
#define __CLONER_H "$Id: cloner.h,v 1.8 2003/07/21 23:36:34 dan_karrels Exp $"

#include	<string>
#include	<vector>
#include	<list>

#include	<ctime>

#include	"client.h"
#include	"iClient.h"
#include	"iServer.h"

using std::string ;
using std::vector ;
using std::list ;

namespace gnuworld
{

class cloner : public xClient
{

public:
	cloner( const string& configFileName ) ;
	virtual ~cloner() ;

	virtual void OnConnect() ;
	virtual void OnTimer( xServer::timerID, void* ) ;
	virtual void OnPrivateMessage( iClient*, const string&,
			bool secure = false ) ;
	virtual void addClone() ;

protected:

	virtual bool		hasAccess( const string& ) const ;

	virtual string		randomNick( int minLength = 5,
					int maxLength = 9 ) ;
	virtual string		randomUser() ;
	virtual string		randomHost() ;
	virtual char		randomChar() ;

	list< string >		allowAccess ;
	list< iClient* >	clones ;
	vector< string >	userNames ;
	vector< string >	hostNames ;
	iServer*		fakeServer ;

	bool			allowOpers ;

	size_t			makeCloneCount ;
	size_t			cloneBurstCount ;
	size_t			minNickLength ;
	size_t			maxNickLength ;

	string			cloneDescription ;
	string			cloneMode ;
	string			fakeServerName ;
	string			fakeServerDescription ;

} ;

} // namespace gnuworld

#endif // __CLONER_H
