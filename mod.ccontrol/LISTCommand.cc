/**
 * LISTCommand.cc
 * Gives list of all kind of stuff to the oper
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
 * $Id: LISTCommand.cc,v 1.13 2005/01/12 03:50:29 dan_karrels Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: LISTCommand.cc,v 1.13 2005/01/12 03:50:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool LISTCommand::Exec( iClient* theClient, const string& Message)
{	 

StringTokenizer st( Message ) ;
if(st.size() < 2)
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("LIST %s\n",st.assemble(1).c_str());
if(!strcasecmp(st[1].c_str(),"glines"))
	{
	bot->listGlines(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"suspended"))
	{
	bot->listSuspended(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"servers"))
	{
	bot->listServers(theClient);
	}

else if(!strcasecmp(st[1].c_str(),"badchannels"))
	{
	bot->listBadChannels(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"exceptions"))
	{
	bot->listExceptions(theClient);
	}
else
	{
	bot->Notice(theClient,"Unknown list");
	}

return true ;
}

}
}

