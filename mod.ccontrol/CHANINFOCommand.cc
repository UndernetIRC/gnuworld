/**
 * CHANINFOCommand.cc
 * Shows information about a channel
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
 * $Id: CHANINFOCommand.cc,v 1.13 2003/06/28 01:21:19 dan_karrels Exp $
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>

#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"

const char CHANINFOCommand_cc_rcsId[] = "$Id: CHANINFOCommand.cc,v 1.13 2003/06/28 01:21:19 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool CHANINFOCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[1].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d chars",channel::MaxName);
	return false;
	}

bot->MsgChanLog("CHANINFO %s\n",st[1].c_str());
	    
Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s",
		st[ 1 ].c_str() ) ;
	return true ;
	}

bot->Notice( theClient, "Channel %s is mode %s",
	st[ 1 ].c_str(),
	theChan->getModeString().c_str() ) ;
bot->Notice( theClient, "Created at time: %d",
	theChan->getCreationTime() ) ;
bot->Notice( theClient, "Number of channel users: %d",
	theChan->size() ) ;
#ifdef TOPIC_TRACK
bot->Notice(theClient,"Topic: %s",
	theChan->getTopic().c_str());
#endif
return true ;
}

}
} // namespace gnuworld
