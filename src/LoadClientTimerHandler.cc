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
 * $Id: LoadClientTimerHandler.cc,v 1.11 2005/01/12 03:50:37 dan_karrels Exp $
 */

#include	<iostream>

#include	"LoadClientTimerHandler.h"
#include	"server.h"
#include	"ELog.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: LoadClientTimerHandler.cc,v 1.11 2005/01/12 03:50:37 dan_karrels Exp $" ) ;

namespace gnuworld
{

void LoadClientTimerHandler::OnTimer( const timerID&, void* )
{
//elog	<< "LoadClientTimerHandler::OnTimer("
//	<< moduleName
//	<< ", "
//	<< configFileName
//	<< ")"
//	<< std::endl ;

// Load the client from the module, attach it to the server, and
// burst it onto the network
if( !theServer->AttachClient( moduleName, configFileName, true ) )
	{
	elog	<< "LoadClientTimer> Failed to load client module: "
		<< moduleName
		<< std::endl ;
	return ;
	}

delete this ;
}

} // namespace gnuworld
