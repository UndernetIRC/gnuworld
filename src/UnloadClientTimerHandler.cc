/**
 * UnloadClientTiemrHandler.cc
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
 * $Id: UnloadClientTimerHandler.cc,v 1.5 2003/06/06 13:14:16 dan_karrels Exp $
 */

#include	<iostream>

#include	"UnloadClientTimerHandler.h"
#include	"server.h"
#include	"ELog.h"

const char UnloadClientTimerHandler_h_rcsId[] = __UNLOADCLIENTTIMERHANDLER_H ;
const char UnloadClientTimerHandler_cc_rcsId[] = "$Id: UnloadClientTimerHandler.cc,v 1.5 2003/06/06 13:14:16 dan_karrels Exp $" ;
const char ELog_h_rcsId[] = __ELOG_H ;

namespace gnuworld
{

int UnloadClientTimerHandler::OnTimer( timerID, void* )
{
//elog	<< "UnloadClientTimerHandler::OnTimer("
//	<< moduleName
//	<< ")"
//	<< std::endl ;

theServer->DetachClient( moduleName, reason ) ;

delete this ;
return 0 ;
}

} // namespace gnuworld
