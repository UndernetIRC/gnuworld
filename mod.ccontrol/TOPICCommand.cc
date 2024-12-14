/**
 * INVITECommand.cc
 * Cause the bot to invite you to a channel
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
 * $Id: INVITECommand.cc,v 1.21 2006/09/26 17:35:58 kewlio Exp $
 */

#include	<string>
#include	<iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"Network.h"
#include	"gnuworld_config.h"

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool TOPICCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if( st.size() < 2 )
	{
	// send help
	Usage( theClient ) ;
	return false ;
	}
if(st[1].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d characters",
		channel::MaxName);
	return false;
	}
ccUser* tmpUser = bot->IsAuth(theClient);
bot->MsgChanLog("TOPIC %s\n",st.assemble(1).c_str());

//If the channel doesnt begin with # add it
string chanName = st[ 1 ] ;
if( chanName[ 0 ] != '#' )
	{
	chanName.insert( chanName.begin(), '#' ) ;
	}

string topic;
if (st.size() == 2)
	topic = "";
else
	topic = "(" + theClient->getNickName() + ") " + st.assemble(2);

if ( topic.size() > 145 )
	{
	bot->Notice(theClient, "Topic cannot exceed 145 chars");
	return true;
	}


Channel* theChan = Network->findChannel(st[1]);
if (theChan == NULL)
	{
	bot->Notice(theClient, "Channel '%s' does not exist!", st[1].c_str());
	return true;
	}


stringstream s;
s	<< bot->getCharYY()
	<< " T "
	<< st[1]
	<< " :"
	<< topic
	<< ends;

bot->Write( s );

return true ;
}

}

}


