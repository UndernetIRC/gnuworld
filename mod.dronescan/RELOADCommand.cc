/**
 * RELOADCommand.cc
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
 * Reloads the bot
 */

#include <time.h>

#include "Network.h"
#include "StringTokenizer.h"

#include "levels.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "sqlUser.h"

#include	<string>
#include	<iostream>

#include	"iClient.h"

/*#include	"Command.h"
#include	"Channel.h"*/
#include	"ELog.h"

namespace gnuworld
{

namespace ds {

void RELOADCommand::Exec( const iClient *theClient, const string& Message, const sqlUser* theUser )
{
//elog	<< "RELOAD> theClient: "
//	<< *theClient
//	<< ", args: "
//	<< args
//	<< std::endl ;
elog	<< "DS.RELOADCommand> Reloading..."
 	<< std::endl ;

bot->Reply( theClient, "Reloading..." ) ;
bot->getUplink()->UnloadClient( bot, "I need more to do" ) ;
bot->getUplink()->LoadClient( "libdronescan.la",
	bot->getConfigFileName() ) ;

}

} // namespace ds

} // namespace gnuworld
