/**
 * ANNOUNCECommand.cc
 * Send global messages
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
 * $Id: ANNOUNCECommand.cc,v 1.2 2009/06/13 06:43:34 hidden1 Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include 	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: ANNOUNCECommand.cc,v 1.2 2009/06/13 06:43:34 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool ANNOUNCECommand::Exec( iClient* theClient, const string& Message )
{	 
StringTokenizer st( Message ) ;
bot->MsgChanLog("ANNOUNCE %s\n",st.assemble(1).c_str());
	
if(st.size() < 2) 
	{
	Usage(theClient);
	return true;
	}

bot->announce(theClient, st.assemble(1));
return true;
}

}
} // namespace gnuworld

