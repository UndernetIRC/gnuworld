/**
 * SHUTDOWNCommand.cc
 * Shuts down the bot
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
 * $Id: SHUTDOWNCommand.cc,v 1.8 2005/01/08 23:33:42 dan_karrels Exp $
 */

#include	<string>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"server.h"

RCSTAG( "$Id: SHUTDOWNCommand.cc,v 1.8 2005/01/08 23:33:42 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SHUTDOWNCommand::Exec( iClient* theClient, const string& Message )
{	 
StringTokenizer st( Message ) ;
	
if(st.size() < 2) 
	{
	Usage(theClient);
	return true;
	}
bot->MsgChanLog("SHUTDOWN %s\n",st.assemble(1).c_str());

server->Shutdown( st.assemble( 1 ) ) ;

return true;
}

}
} // namespace gnuworld

