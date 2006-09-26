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
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"Network.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: INVITECommand.cc,v 1.21 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

// invite #channel
bool INVITECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if( st.size() == 1 )
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
bot->MsgChanLog("INVITE %s\n",st.assemble(1).c_str());

//If the channel doesnt begin with # add it 
string chanName = st[ 1 ] ;
if( chanName[ 0 ] != '#' )
	{
	chanName.insert( chanName.begin(), '#' ) ;
	}

iClient* inviteClient = 0 ;
if(st.size() > 2)
	{
	// Invite a different user
	inviteClient = Network->findNick(st[2]);
	if( inviteClient == NULL)
		{
		bot->Notice(theClient,"I cant find '%s' anywhere",
			st[2].c_str());
		return true;
		}

	if( !tmpUser  && (inviteClient != theClient))
		{
		bot->Notice(theClient,"You must login to invite someone else!");
		return true;
		}
	}
else
	{
	// Invite the requesting user
	inviteClient = theClient ;
	}

Channel* theChan = Network->findChannel(st[1]);
if (theChan == NULL)
{
	bot->Notice(theClient, "Channel '%s' does not exist!", st[1].c_str());
	return true;
}

// xClient::Invite() will Join/Part the channel if necessary.
return bot->Invite( inviteClient, chanName ) ;
}

}
}

