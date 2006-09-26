/**
 * KICKCommand.cc
 * Kick a user from a channel
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
 * $Id: KICKCommand.cc,v 1.15 2006/09/26 17:35:58 kewlio Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"ccBadChannel.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: KICKCommand.cc,v 1.15 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

// kick #channel nick reason
bool KICKCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 4 )
	{
	Usage( theClient ) ;
	return true ;
	}

//Check if the channel begins with # , if not add it
string chanName = st[ 1 ] ;
if( chanName[ 0 ] != '#' )
	{
	chanName.insert( chanName.begin(), '#' ) ;
	}

if(chanName.size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d characters",
		channel::MaxName);
	}

bot->MsgChanLog("KICK %s\n",st.assemble(1).c_str());

ccBadChannel* Chan = bot->isBadChannel(st[1]);
if(Chan)
        {
        bot->Notice(theClient,"Sorry, you can't kick from  "
                             "this channel because: %s",
                             Chan->getReason().c_str());
        return false;
        }

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel: %s",
		chanName.c_str() ) ;
	return true ;
	}

iClient* Target = Network->findNick( st[ 2 ] ) ;
if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find nick: %s",
		st[ 2 ].c_str() ) ;
	return true ;
	}

if( NULL == theChan->findUser( Target ) )
	{
	bot->Notice( theClient, "User %s was not found "
		"on channel %s",
		st[ 1 ].c_str(),
		theChan->getName().c_str());
	return true ;
	}

if( Target->getMode(iClient::MODE_SERVICES))
	{
	bot->Notice(theClient,"Are you trying to get me in trouble with %s?",st[ 2 ].c_str());
	return false;
	}
bot->Notice( theClient, "Kicking %s from channel %s because %s",
	Target->getNickName().c_str(),
	chanName.c_str(),
	st.assemble( 3 ).c_str() ) ;

//If its an oper channel , no need to join and part the channel , so just kick the user
if( bot->isOperChan( chanName ) )
	{
	bot->Kick( theChan, Target, st.assemble( 3 ) ) ;
	return true ;
	}
//Its not an oper channel 
bot->Join( chanName, string(), 0, true ) ;

bot->Kick( theChan, Target, st.assemble( 3 ) ) ;

bot->Part( chanName ) ;

return true ;
}

}
}
