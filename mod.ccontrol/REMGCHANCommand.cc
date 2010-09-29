/**
 * REMGCHANCommand.cc
 * Removes a BADCHAN gline
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
 * $Id: REMGCHANCommand.cc,v 1.14 2006/09/26 17:36:00 kewlio Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: REMGCHANCommand.cc,v 1.14 2006/09/26 17:36:00 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

// remgline user@host
bool REMGCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

if((st[1].substr(0,1) != "#") || (st[1].size() > channel::MaxName))
	{
	bot->Notice(theClient,"Invalid channel name. It must begin with # and can't be "
			"more than %d characters in length.",
			channel::MaxName);
	return false;
	}
bot->MsgChanLog("REMGCHAN %s\n",st.assemble(1).c_str());

ccGline *tmpGline = bot->findGline(bot->removeSqlChars(st[1]));
if(tmpGline != NULL)
	{
	if(!tmpGline->Delete())
		bot->MsgChanLog("Error while removing gline for channel %s from the db\n",st[1].c_str());
	bot->remGline(tmpGline);
	delete tmpGline;
	}	
//bot->setRemoving(st[1]);
server->removeGline( st [ 1 ] ,bot);
//bot->unSetRemoving();
return true ;
}

}
} // namespace gnuworld

