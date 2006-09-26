/**
 * DEOPCommand.cc
 * Cause the bot to deop users on a specific channel
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
 * $Id: DEOPCommand.cc,v 1.16 2006/09/26 17:35:58 kewlio Exp $
 */

#include	<set>
#include	<string>
#include	<vector>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"Constants.h"
#include	"ccBadChannel.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: DEOPCommand.cc,v 1.16 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool DEOPCommand::Exec( iClient* theClient, const string& Message )
{
bool foundUser = false;

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[1].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d characters",
		channel::MaxName);
	return false;
	}
Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}

ccBadChannel* Chan = bot->isBadChannel(st[1]);
if(Chan)
        {
        bot->Notice(theClient,"Sorry, but you can not change modes in "
                             "this channel because: %s",
                             Chan->getReason().c_str());
        return false;
        }

iClient* Target = 0;


bot->MsgChanLog("DEOP %s\n",st.assemble(1).c_str());
string modes = "-";
string args = "";

for( StringTokenizer::size_type i = 2 ; i < st.size() ; ++i )
	{
	if(st[i].size() > 64)
		{
		bot->Notice(theClient,"Nick can't be more than 64 characters");
		return false;
		}

	Target = Network->findNick( st[ i ] ) ;
	if( 0 == Target )
		{
		continue ;
		}

	// Check if the user is in the channel and he's not already opped
	ChannelUser* tmpChanUser = theChan->findUser(Target) ;
	if( !tmpChanUser )
		{
		continue ;
		}

	if( Target->getMode( iClient::MODE_SERVICES ) )
		{
		// Don't op services clients, they can op themselves.
		continue ;
		}
	/* ok, the user is in the channel, flag it */
	foundUser = true;
	modes += "o";
	args += st[i] + " ";
	} // for

if (!foundUser)
{
	bot->Notice(theClient, "I couldn't find anyone to deop in %s!",
		st[1].c_str());
	return false;
}

if(  !args.empty() )
	{
	bot->Mode(theChan,modes,args,true);
	}
return true;
}

}
}			
	
