/**
 * REOPCommand.cc 
 * Clears all channel ops, and reops the specified nick
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
 * $Id: REOPCommand.cc,v 1.5 2005/01/12 03:50:29 dan_karrels Exp $
 */

#include	<string>

#include	<cstdlib>

#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"ccBadChannel.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: REOPCommand.cc,v 1.5 2005/01/12 03:50:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::vector ;
using gnuworld::iClient;

namespace uworld
{

bool REOPCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[1].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d chars",channel::MaxName);
	return false;
	}

Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s",
		st[ 1 ].c_str() ) ;
	return true ;
	}

if(bot->isOperChan(theChan))
	{
	bot->Notice(theClient,"C'mon , you know you cant reop an oper channel");
	return false;
	}

iClient* reopClient = Network->findNick(st[2]);
if(!reopClient)
	{
	bot->Notice(theClient,"I cant find %s anywere",st[2].c_str());
	return true;
	}

bot->MsgChanLog("REOP %s\n",st.assemble(1).c_str());
ccBadChannel* Chan = bot->isBadChannel(st[1]);
if(Chan)
        {
        bot->Notice(theClient,"Sorry, but you can not change modes in "
                             "this channel because : %s"
                             ,Chan->getReason().c_str());
        return false;
        }

// First, clear all ops
bot->ClearMode( theChan, string( "o" ), true ) ;

// Now, op the proper user
bot->Op( theChan, reopClient ) ;

return true;	
}

}
}
