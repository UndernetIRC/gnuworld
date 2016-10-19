/**
 * IP6Command.cc
 * Modify the ipv6 connection limits
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
 * $Id: IP6Command.cc,v 1.3 2008/12/28 12:21:15 hidden1 Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include 	"Constants.h"
#include	"gnuworld_config.h"
#include	"ccException.h"
#include	"Network.h"

RCSTAG( "$Id: IP6Command.cc,v 1.3 2008/12/28 12:21:15 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool IP6Command::Exec( iClient* theClient, const string& Message )
{	 
StringTokenizer st( Message ) ;
ccIp6isp* Ip6isp;
//ccIp6nb* Ip6nb;
	
if(st.size() < 2) 
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("IP6 %s\n",st.assemble(1).c_str());
if(!strcasecmp(st[1].c_str(),"list"))
	{
	bot->listIp6Exceptions(theClient);
	return true;
	}
else if(!strcasecmp(st[1].c_str(),"addisp"))
	{
	if(st.size() < 5) 
		{
		bot->Notice(theClient,"SYNTAX: ADDISP <name> <max connections> <clones cidr> [abuse email]");
		//bot->Notice(theClient, "-f: forcecount - use it if you want limits to still be enforced even if the netblocks"
		//	" of this isp are not the closest match for a client");
		//bot->Notice(theClient, "-i: inactive - deactivates glines, reports only");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp name cannot exceeed 32 characters");
		return true;
		}
	if (atoi(st[3].c_str()) == 0)
		{
		bot->Notice(theClient,"SYNTAX: ADDISP <name> <max connections> <clones cidr> [abuse email]");
		return true;
		}

	Ip6isp = bot->getIp6isp(st[2]);
	
	if (Ip6isp != 0)
		{
		bot->Notice(theClient,"An isp already exists with that name.");
		return true;
		}

	else
		{
		string email = (st.size() >= 5 ? st[4] : "none@available");
		if(bot->insertIp6isp(theClient,st[2],atoi(st[3].c_str()),atoi(st[4].c_str()),email, 1, 0))
			{
			bot->Notice(theClient,"Successfully added isp '%s' for %d connections per /%d"
				    ,st[2].c_str(),atoi(st[3].c_str()),atoi(st[4].c_str()));
			}
		}
	}
else if(!strcasecmp(st[1].c_str(),"addnetblock"))
	{
	if(st.size() != 4) 
		{
		bot->Notice(theClient,"SYNTAX: ADDNETBLOCK <isp> <netblock>");
		return true;
		}
	Ip6isp = bot->getIp6isp(st[2]);
	
	if (Ip6isp == 0)
		{
		bot->Notice(theClient,"Isp not found. Use ADDISP first");
		return true;
		}
	else
		{
		if(bot->insertIp6nb(theClient,st[3],Ip6isp->getID(), false))
			{
			bot->Notice(theClient,"Successfully added '%s' to '%s'"
				    ,st[3].c_str(),st[2].c_str());
			}
		}
	}
else if(!strcasecmp(st[1].c_str(),"active"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: ACTIVE <isp> <yes|no>");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	Ip6isp = bot->getIp6isp(st[2]);
	
	if (Ip6isp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	int res;
	if (!strcasecmp(st[3].c_str(), "yes"))
		res = 1;
	else if (!strcasecmp(st[3].c_str(), "no"))
		res = 0;
	else 
		{
		bot->Notice(theClient,"must be yes or no");
		return true;
		}
	Ip6isp->setActive(res);
	Ip6isp->setModOn(::time(0));
	Ip6isp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
	bot->reloadIp6isp(theClient, Ip6isp);
	if (!Ip6isp->updateData()) 
		{
		bot->Notice(theClient, "SQL insertion failed.");
		}
	else
		bot->Notice(theClient,"G-lines on %s %sactivated", Ip6isp->getName().c_str(), res ? "" : "de");
	}
else if(!strcasecmp(st[1].c_str(),"forcecount"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: FORCECOUNT <isp> <yes|no>");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	Ip6isp = bot->getIp6isp(st[2]);
	
	if (Ip6isp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	int res;
	if (!strcasecmp(st[3].c_str(), "yes"))
		res = 1;
	else if (!strcasecmp(st[3].c_str(), "no"))
		res = 0;
	else 
		{
		bot->Notice(theClient,"must be yes or no");
		return true;
		}
	Ip6isp->setForcecount(res);
	Ip6isp->setModOn(::time(0));
	Ip6isp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
	bot->reloadIp6isp(theClient, Ip6isp);
	if (!Ip6isp->updateData()) 
		{
		bot->Notice(theClient, "SQL insertion failed.");
		}
	else
		bot->Notice(theClient,"forcecount for %s is now %sabled", Ip6isp->getName().c_str(), res ? "en" : "dis");
	}
else if(!strcasecmp(st[1].c_str(),"chccidr"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: CHCCIDR <isp> <new CloneCidr>");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	Ip6isp = bot->getIp6isp(st[2]);
	
	if (Ip6isp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	else
		{
		if (!bot->ip6cidrChangeCheck(theClient, Ip6isp, atoi(st[3]))) 
			return true;
		Ip6isp->setCloneCidr(atoi(st[3].c_str()));
		Ip6isp->setModOn(::time(0));
		Ip6isp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
		bot->reloadIp6isp(theClient, Ip6isp);
		if (!Ip6isp->updateData()) 
			{
			bot->Notice(theClient, "SQL insertion failed.");
			}
		else
			bot->Notice(theClient,"%s is now checking clones on /%ds. Limit: %d", Ip6isp->getName().c_str(), atoi(st[3].c_str()), Ip6isp->getLimit());
		}
	}
else if(!strcasecmp(st[1].c_str(),"chlimit"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: CHLIMIT <isp> <limit>");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	Ip6isp = bot->getIp6isp(st[2]);
	
	if (Ip6isp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	else
		{
		Ip6isp->setLimit(atoi(st[3].c_str()));
		Ip6isp->setModOn(::time(0));
		Ip6isp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
		if (!Ip6isp->updateData()) 
			{
			bot->Notice(theClient, "SQL insertion failed.");
			}
		else
			bot->Notice(theClient,"New limit for %s is now %d", Ip6isp->getName().c_str(), Ip6isp->getLimit());
		}
	}
else if(!strcasecmp(st[1].c_str(),"chname"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: CHNAME <isp> <new name>");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't be more than 32 characters");
		return true;
		}
	Ip6isp = bot->getIp6isp(st[3]);
	
	if (Ip6isp != 0)
		{
		bot->Notice(theClient,"Isp %s already exists.", Ip6isp->getName().c_str());
		return true;
		}
	Ip6isp = bot->getIp6isp(st[2]);
	
	if (Ip6isp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	else
		{
		Ip6isp->setName(ccontrol::removeSqlChars(st[3]));
		Ip6isp->setModOn(::time(0));
		Ip6isp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
		if (!Ip6isp->updateData()) 
			{
			bot->Notice(theClient, "SQL insertion failed.");
			}
		else
			bot->Notice(theClient,"Renamed %s to %s", st[2].c_str(), Ip6isp->getName().c_str());
		}
	}
else if(!strcasecmp(st[1].c_str(),"delisp"))
	{
	if(st.size() < 3) 
		{
		bot->Notice(theClient,"You must specify the isp you want to delete");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	else
		{
		if(bot->delIp6isp(theClient,st[2]))
			{
			bot->Notice(theClient,"Successfully deleted isp '%s'",st[2].c_str());
			}
		
		}
	}
else if(!strcasecmp(st[1].c_str(),"clearall"))
	{
	if (st.size() < 3) {
		bot->Notice(theClient, "Are you sure? Use CLEARALL -f");
		return true;
	}
	if (strcasecmp(st[2].c_str(),"-f")) {
		bot->Notice(theClient, "Are you sure? Use CLEARALL -f");
		return true;
	}

	if(bot->clearShells(theClient))
		{
		bot->Notice(theClient,"Successfully deleted all isps");
		}
	}

else if(!strcasecmp(st[1].c_str(),"test"))
	{
	if (st.size() > 2)
		{
		//bot->test(theClient, st[2]);
		}
	return true;

	}
	
//ip6userInfo(

else if(!strcasecmp(st[1].c_str(),"userinfo"))
	{
	if(st.size() < 3) 
		{
		bot->Notice(theClient,"Syntax: USERINFO <nick>");
		return true;
		}
	iClient* Target = Network->findNick(st[2]) ;
	if( NULL == Target )
		{
		bot->Notice( theClient, "Unable to find nick: %s", st[ 2 ].c_str() ) ;
		return true ;
		}
	return bot->ip6userInfo(theClient, Target);
	}

else if(!strcasecmp(st[1].c_str(),"delnetblock"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"Syntax: DELNETBLOCK <isp> <netblock>");
		return true;
		}
	else
		{
		if(bot->delIp6nb(theClient,st[2],st[3], false))
			{
			bot->Notice(theClient,"Successfully deleted netblock '%s'",st[2].c_str());
			}
		}

	} 
else {
	/* unknown command */
	Usage(theClient);
	return true;
}
return true;
}


}
} // namespace gnuworld


