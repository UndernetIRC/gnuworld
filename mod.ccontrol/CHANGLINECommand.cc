/**
 * CHANGLINECommand.cc
 * Glines a specific channel
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
 * $Id: CHANGLINECommand.cc,v 1.1 2005/10/04 01:45:30 kewlio Exp $
 */

#include	<string>
#include        <iomanip>
#include	<map>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"misc.h"
#include	"Gline.h"
#include	"ip.h"
#include	"ELog.h"
#include	"Gline.h"
//#include	"gline.h"
#include 	"time.h"
#include	"ccUser.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: CHANGLINECommand.cc,v 1.1 2005/10/04 01:45:30 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string;

// Input: changline #blah reason
//
// Output: C GL * +*@lamer.net 3600 :Banned (*@lamer) ...
//

namespace uworld
{

bool CHANGLINECommand::Exec( iClient* theClient, const string& Message )
{
	StringTokenizer st(Message);

	if (!dbConnected)
	{
		bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
		return false;
	}

	if (st.size() < 4)
	{
		Usage(theClient);
		return true;
	}

	StringTokenizer::size_type pos = 1;
	time_t gLength = bot->getDefaultGlineLength();
	ccUser* tmpUser = bot->IsAuth(theClient);

	/* log use of the command */
	bot->MsgChanLog("CHANGLINE %s\n",st.assemble(1).c_str());

	/* make sure they're trying a channel gline! */
	if (st[pos].substr(0,1) != "#")
	{
		bot->Notice(theClient,"Umm... this is CHANGLINE, not GLINE - Try glining a channel maybe?");
		return true;
	}

	string userName;
	string hostName;
	string Length;

	Length.assign(st[2]);
	unsigned int Units = 1;		// Default for seconds
	unsigned int ResStart = 2;

	if (!strcasecmp(Length.substr(Length.length()-1).c_str(),"d"))
	{
		Units = (24*3600);
		Length.resize(Length.length()-1);
	}
	else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"h"))
	{
		Units = 3600;
		Length.resize(Length.length()-1);
	}
	else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"m"))
	{
		Units = 60;
		Length.resize(Length.length()-1);
	}
	else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"s"))
	{
		Units = 1;
		Length.resize(Length.length()-1);
	}

	gLength = (atoi(Length.c_str()) * Units);
	if (gLength == 0) 
	{
		gLength = bot->getDefaultGlineLength();
		ResStart = 1;
	}

	string nickUserHost = theClient->getRealNickUserHost();
	
	if (!tmpUser)
	{
		bot->Notice(theClient,"You must login to issue this channel gline!");
		return false;
	}

	typedef map<string , int> GlineMapType;
	GlineMapType glineList;

	if (st[1].size() > channel::MaxName)
	{
		bot->Notice(theClient,"Channel name can't be more than %d chars",channel::MaxName);
		return false;
	}

	Channel* theChan = Network->findChannel(st[1]);
	if (NULL == theChan)
	{
		bot->Notice(theClient, "Unable to find channel %s",
			st[1].c_str());
		return true;
	}

	ccGline *TmpGline;
	iClient *TmpClient;
	string curIP;
	GlineMapType::iterator gptr;
	for (Channel::const_userIterator ptr = theChan->userList_begin();
		ptr != theChan->userList_end(); ++ptr)
	{
		TmpClient = ptr->second->getClient();
		curIP = xIP(TmpClient->getIP()).GetNumericIP();
		gptr = glineList.find("*@" + curIP);
		if (gptr != glineList.end())
		{
			/* duplicate gline - continue to next channel user */
			continue;
		}
		if ((!TmpClient->getMode(iClient::MODE_SERVICES)) &&
			!(bot->IsAuth(TmpClient)) && !(TmpClient->isOper())) 
		{
			/* create a new gline and queue it */
			TmpGline = new ccGline(bot->SQLDb);
			assert(TmpGline != NULL);
			TmpGline->setHost("*@"  + curIP);
			TmpGline->setExpires(unsigned(gLength));
			TmpGline->setAddedBy(nickUserHost);
			TmpGline->setReason(st.assemble( pos + ResStart ));
			TmpGline->setAddedOn(::time(0));
			TmpGline->setLastUpdated(::time(0));
			bot->queueGline(TmpGline);
		}
	}

	return true;
}

}
}
