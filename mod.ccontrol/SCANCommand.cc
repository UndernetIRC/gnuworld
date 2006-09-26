/**
 * SCANCommand.cc
 * Shows alist of users which match a certain host/real name
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
 * $Id: SCANCommand.cc,v 1.14 2006/09/26 17:36:01 kewlio Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	<map>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"match.h"
#include	"Network.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: SCANCommand.cc,v 1.14 2006/09/26 17:36:01 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SCANCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
	

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("SCAN %s\n",st.assemble(1).c_str());

typedef list<const iClient*> clientsList;

bool showUsers = false;
bool showIdentReport = false;
bool realHostLook = false;
bool fakeHostLook  = false;
bool nameLook = false;
unsigned int pos = 1;
string userName;
string hostName;
string realName;
string ClientInfo;
const iClient* curClient;
typedef map<string,int> identMapType;
identMapType identMap;
clientsList cList;

for(pos = 1; pos < st.size() ; )
	{
	if(!strcasecmp(st[pos],"-v"))
		{
		showUsers = true;
		++pos;
		}
	else if(!strcasecmp(st[pos],"-i"))
		{
		showIdentReport = true;
		++pos;
		}
	else if(!strcasecmp(st[pos],"-h"))
		{
		if(pos +1 >= st.size())
			{
			bot->Notice(theClient,"-h must get a host to scan for");
			return true;
			}
		if((nameLook) || (fakeHostLook))
			{
			bot->Notice(theClient,"You can't specify more than one scanning option");
			return true;
			}
		realHostLook = true;
		hostName = st[pos+1];
		pos+=2;
		}
	else if(!strcasecmp(st[pos],"-fh"))
		{
		if(pos +1 >= st.size())
			{
			bot->Notice(theClient,"-fh must get a host to scan for");
			return true;
			}
		if((nameLook) || (realHostLook))
			{
			bot->Notice(theClient,"You can't specify more than one scanning option");
			return true;
			}
		fakeHostLook = true;
		hostName = st[pos+1];
		pos+=2;
		}

	else if(!strcasecmp(st[pos],"-n"))
		{
		if(pos +1 >= st.size())
			{
			bot->Notice(theClient,"-n must get a real name to scan for");
			return true;
			}
		if((realHostLook) || (fakeHostLook))
			{
			bot->Notice(theClient,"You can't specify more than one scanning option");
			return true;
			}
		nameLook = true;
		realName = st[pos+1];
		pos+=2;
		}
	else
		{
		bot->Notice(theClient,"SCAN command can only get -h <host>, -fh <host> or -n <real name>");
		return true;
		}
	}
if(fakeHostLook)
	{
	if(string::npos == hostName.find_first_of("@"))
		{
		hostName = string("*@" + hostName);
		}
	cList = Network->matchUserHost(hostName);
	}
else if(realHostLook)
	{
	if(string::npos == hostName.find_first_of("@"))
		{
		hostName = string("*@" + hostName);
		}
	cList = Network->matchRealUserHost(hostName);
	}
else if(nameLook)
	{
	cList = Network->matchRealName(realName);
	}
else 
	{
	bot->Notice(theClient,"You didn't specify a search type (-h/-fh/-n) - please try again");
	return true;
	}
if( (cList.size() > scan::MAX_SHOW) && (showUsers))	
	{
	bot->Notice(theClient,"There were %d users matching this search, only 15 will be shown",
		cList.size());
	}
else
	{
	bot->Notice(theClient,"There were %d users matching this search",cList.size());
	}

unsigned int shown = 0;
unsigned int ident_nonidented = 0;
unsigned int ident_count = 0;
	
if(showUsers)
	{				
	const ChannelUser* curChannel;
	string ClientInfo;
	for(clientsList::iterator cptr = cList.begin();((cptr!=cList.end()) && (shown < 15));++cptr)
		{
		curClient = *cptr;
		ClientInfo = curClient->getNickUserHost();
		if(!curClient->getMode(iClient::MODE_SERVICES)) //Dont show channels for +k users
			{
			for( iClient::const_channelIterator ptr = curClient->channels_begin() ;
    			ptr != curClient->channels_end() ; ++ptr )
    				{  
    				ClientInfo += " ";
    				curChannel = (*ptr)->findUser(const_cast< iClient* >(curClient));
				if(curChannel->isModeO())
					{
					ClientInfo += "@";
					}
	    			if(curChannel->isModeV())
					{
					ClientInfo += "+";
					}
				ClientInfo += (*ptr)->getName();				
				}
			++shown;
			}
		bot->Notice(theClient,"%s",ClientInfo.c_str());
		}
	}

/* do ident report here (if selected) */
if(showIdentReport)
{
	/* ok, show the report */
	for (clientsList::iterator cptr = cList.begin(); (cptr!=cList.end()); ++cptr)
	{
		/* cycle the userlist and gather stats */
		curClient = *cptr;
		ClientInfo = curClient->getNickUserHost();
		userName = curClient->getUserName();
		if (userName[0]=='~')
		{
			/* unidented client */
			ident_nonidented++;
		} else {
			/* idented client */
			identMap[userName]++;
			if (identMap[userName] == 1)
				ident_count++;
		}
	}
	bot->Notice(theClient, "Ident report: %d unidented clients, %d unique idents",
		ident_nonidented, ident_count);
}
		
return true;
}

}
}
