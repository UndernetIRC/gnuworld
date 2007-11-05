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
 * $Id: CHANINFOCommand.cc,v 1.20 2007/11/05 10:04:23 kewlio Exp $
 */

#include	<string>

#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"ccontrol_generic.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: CHANINFOCommand.cc,v 1.20 2007/11/05 10:04:23 kewlio Exp $" ) ;

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
	bot->Notice(theClient,"Channel name can't be more than %d characters",
		channel::MaxName);
	return false;
	}

ccUser* tmpAuth = bot->IsAuth(theClient);
if (!tmpAuth)
	return false;
unsigned int OpFlag = tmpAuth->getType();

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
bot->Notice( theClient, "Created at time: %d (%s ago)",
	theChan->getCreationTime(), Ago(theChan->getCreationTime()));

/* iterate the channel user list to get statistics on each mode
   and possibly a user list in future */
int totalOps = 0; 
int totalVoice = 0;
string tmpMode;

for (Channel::userIterator userItr = theChan->userList_begin();
	userItr != theChan->userList_end(); ++userItr)
{
	ChannelUser* theUser = userItr->second;

	if (theUser->isModeO())
		totalOps++;
	if (theUser->isModeV())
		totalVoice++;
	if (OpFlag == operLevel::CODERLEVEL)
	{
		/* show full info to coders - make it format for easy viewing */
		if (theUser->isModeO() && theUser->isModeV())
			tmpMode = "+o+v: ";
		else if (theUser->isModeO())
			tmpMode = "+o:   ";
		else if (theUser->isModeV())
			tmpMode = "+v:   ";
		else
			tmpMode = "none: ";
		bot->Notice(theClient, "%s%s!%s@%s",
			tmpMode.c_str(),
			theUser->getNickName().c_str(),
			theUser->getUserName().c_str(),
			theUser->getHostName().c_str());
	}
}

bot->Notice( theClient, "Number of channel users: %d (%d ops, %d voice)",
	theChan->size(), totalOps, totalVoice);

#ifdef TOPIC_TRACK
bot->Notice(theClient,"Topic: %s",
	theChan->getTopic().c_str());
if (theChan->getTopicTS() != 0)
{
	bot->Notice(theClient, "Topic set %s ago [%ld] by %s",
		Ago(theChan->getTopicTS()),
		theChan->getTopicTS(),
		theChan->getTopicWhoSet().c_str());
}
#endif
return true ;
}

}
} // namespace gnuworld
