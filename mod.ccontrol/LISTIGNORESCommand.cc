/**
 * LISTIGNORESCommand.cc 
 * Shows all ignored masks
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
 * $Id: LISTIGNORESCommand.cc,v 1.10 2006/09/26 17:35:59 kewlio Exp $
 */

#include	<string>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: LISTIGNORESCommand.cc,v 1.10 2006/09/26 17:35:59 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;

namespace uworld
{

bool LISTIGNORESCommand::Exec( iClient* theClient, const string& )
{
bot->MsgChanLog("LISTIGNORES \n");
bot->listIgnores(theClient);
return true;
}

}
} // namespace gnuworld

