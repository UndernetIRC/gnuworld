/*
 * SCANCommand.cc
 *
 * Shows alist of users which match a certain host/real name
 *
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"match.h"
#include	"Network.h"

const char SCANCommand_cc_rcsId[] = "$Id: SCANCommand.cc,v 1.7 2002/03/01 18:27:36 mrbean_ Exp $";

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
bool hostLook = false;
bool nameLook = false;
unsigned int pos = 1;
string hostName;
string realName;

clientsList cList;

for(pos = 1; pos < st.size() ; )
	{
	if(!strcasecmp(st[pos],"-v"))
		{
		showUsers = true;
		++pos;
		}
	else if(!strcasecmp(st[pos],"-h"))
		{
		if(pos +1 >= st.size())
			{
			bot->Notice(theClient,"-h must get a host to scan for");
			return true;
			}
		if(nameLook)
			{
			bot->Notice(theClient,"You can't specify both -h and -n");
			return true;
			}
		hostLook = true;
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
		if(hostLook)
			{
			bot->Notice(theClient,"You can't specify both -h and -n");
			return true;
			}
		nameLook = true;
		realName = st[pos+1];
		pos+=2;
		}
	else
		{
		bot->Notice(theClient,"SCAN command can only get -h <host> or -n <real name>");
		return true;
		}
	}
if(hostLook)
	{
	if(string::npos == hostName.find_first_of("@"))
		{
		hostName = string("*@" + hostName);
		}
	cList = Network->matchUserHost(hostName);
	}
else if(nameLook)
	{
	cList = Network->matchRealName(realName);
	}
else
	{
	bot->Notice(theClient,"You didnt specify a search type (-h/-n) please try again");
	return true;
	}
//TODO: make this a constant
if((cList.size() > 15) && (showUsers))	
	{
	bot->Notice(theClient,"There were %d users matching this search, only 15 will be shown"
		    ,cList.size());
	}
else
	{
	bot->Notice(theClient,"There were %d users matching this search",cList.size());
	}

unsigned int shown = 0;
	
if(showUsers)
	{				
	const iClient* curClient;
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
		
return true;
}

}
}
