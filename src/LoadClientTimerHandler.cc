/**
 * LoadClientTimerHandler.cc
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
 * $Id: LoadClientTimerHandler.cc,v 1.5 2002/06/07 14:38:19 dan_karrels Exp $
 */

#include	<iostream>

#include	"LoadClientTimerHandler.h"
#include	"server.h"
#include	"ELog.h"

const char LoadClientTimerHandler_h_rcsId[] = __LOADCLIENTTIMERHANDLER_H ;
const char LoadClientTimerHandler_cc_rcsId[] = "$Id: LoadClientTimerHandler.cc,v 1.5 2002/06/07 14:38:19 dan_karrels Exp $" ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char server_h_rcsId[] = __SERVER_H ;

namespace gnuworld
{

int LoadClientTimerHandler::OnTimer( timerID, void* )
{
elog	<< "LoadClientTimerHandler::OnTimer("
	<< moduleName
	<< ", "
	<< configFileName
	<< ")"
	<< std::endl ;

// Load the client from the module, attach it to the server, and
// burst it onto the network
if( !theServer->AttachClient( moduleName, configFileName, true ) )
	{
	elog	<< "LoadClientTimer> Failed to load client module: "
		<< moduleName
		<< endl ;
	return -1 ;
	}

delete this ;
return 0 ;
}

} // namespace gnuworld
