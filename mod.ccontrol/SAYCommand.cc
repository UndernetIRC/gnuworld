/**
 * SAYCommand.cc
 * Forces the bot to quote a command
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
 * $Id: SAYCommand.cc,v 1.8 2006/09/26 17:36:01 kewlio Exp $
 */

#include	<string>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: SAYCommand.cc,v 1.8 2006/09/26 17:36:01 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SAYCommand::Exec( iClient* theClient, const string& Message)
{	 

StringTokenizer st( Message ) ;
string Numeric;
string Target;
if(st.size() < 4)
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("%s\n",st.assemble(0).c_str());
if(!strcasecmp(st[1].c_str(),"-s"))
	{
	Numeric = bot->getUplink()->getCharYY();
	}
else if(!strcasecmp(st[1].c_str(),"-b"))
	{
	Numeric = bot->getCharYYXXX();
	}
else
	{
	bot->Notice(theClient,"First argument must be -s for server, or -b for bot");
	return true;
	}

if(!strcasecmp(st[2].substr(0,1),"#"))
	{
	if(!Network->findChannel(st[2]))
		{
		bot->Notice(theClient,"Sorry, but i can't find channel %s",
			    st[2].c_str());
		return true;
		}
	else
		{
		Target = st[2];
		}
	}
else	
	{
	iClient* tClient = Network->findNick(st[2]);
	if(!tClient)
		{
		bot->Notice(theClient,"Sorry, but i can't find nick: %s",
			    st[2].c_str());
		return true;
		}
	else
		{
		Target = tClient->getCharYYXXX();
		}
	}

if (!match("SAY",st[0]))
{
	bot->Write("%s P %s :%s",
		Numeric.c_str(),
		Target.c_str(),
		st.assemble(3).c_str());
} else {
	bot->Write("%s P %s :%cACTION %s%c",
		Numeric.c_str(),
		Target.c_str(),
		1, st.assemble(3).c_str(), 1);
}
return true ;
}

}
}

