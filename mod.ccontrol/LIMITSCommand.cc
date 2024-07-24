/**
 * LIMITSCommand.cc
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
 * $Id: LIMITSCommand.cc,v 1.3 2008/12/28 12:21:15 hidden1 Exp $
 */

#include	<string>
#include	<iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include 	"Constants.h"
#include	"gnuworld_config.h"
#include	"ccException.h"
#include	"Network.h"

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool LIMITSCommand::Exec( iClient* theClient, const string& Message )
{	 
StringTokenizer st( Message ) ;
ccIpLisp* IpLisp;
//ccIpLnb* IpLnb;
	
if(st.size() < 2) 
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("LIMITS %s\n",st.assemble(1).c_str());
if(!strcasecmp(st[1].c_str(),"list"))
	{
	bot->listIpLExceptions(theClient);
	return true;
	}
if(!strcasecmp(st[1].c_str(),"list-old"))
	{
	bool listEmail = false;
	if ((st.size() > 2) && (!strcasecmp(st[2].c_str(), "-e"))) {
		listEmail = true;
	}
	bot->listIpLExceptionsOld(theClient, "", listEmail);
	return true;
	}
if(!strcasecmp(st[1].c_str(),"info"))
	{
	if (st.size() < 3) {
		bot->Notice(theClient,"SYNTAX: info <isp>");
		return true;
	}
	bot->listIpLExceptionsOld(theClient, st[2], true);
	return true;
	}
else if(!strcasecmp(st[1].c_str(),"addisp"))
	{
	if(st.size() < 5) 
		{
		bot->Notice(theClient,"SYNTAX: ADDISP <name> <max connections> <clones cidr> [abuse email]");
		//bot->Notice(theClient, "-f: forcecount - use it if you want limits to still be enforced even if the netblocks"
		//	" of this isp are not the closest match for a client");
		//bot->Notice(theClient, "-i: inactive - deactivates glines and does not prevent new clients from connecting using iauth. It does reports only.");
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

	IpLisp = bot->getIpLisp(st[2]);
	
	if (IpLisp != 0)
		{
		bot->Notice(theClient,"An isp already exists with that name.");
		return true;
		}

	else
		{
		string email = (st.size() > 5 ? st[5] : "none@available");
		if(bot->insertIpLisp(theClient,st[2],atoi(st[3].c_str()),atoi(st[4].c_str()),email, 1, 0))
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
	IpLisp = bot->getIpLisp(st[2]);
	
	if (IpLisp == 0)
		{
		bot->Notice(theClient,"Isp not found. Use ADDISP first");
		return true;
		}
	else
		{
		if(bot->insertIpLnb(theClient,st[3],IpLisp->getID(), false))
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
	IpLisp = bot->getIpLisp(st[2]);

	if (IpLisp == 0)
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
	IpLisp->setActive(res);
	IpLisp->setModOn(::time(0));
	IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
	bot->reloadIpLisp(theClient, IpLisp);
	if (!IpLisp->updateData())
		{
		bot->Notice(theClient, "SQL insertion failed.");
		}
	else
		bot->Notice(theClient,"Limits are %s enforced on %s", res ? "now" : "no longer", IpLisp->getName().c_str());
	}

else if(!strcasecmp(st[1].c_str(),"nogline"))
	{
	if(st.size() < 4)
		{
		bot->Notice(theClient,"SYNTAX: NOGLINE <isp> <yes|no>");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	IpLisp = bot->getIpLisp(st[2]);

	if (IpLisp == 0)
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
	IpLisp->setNoGline(res);
	IpLisp->setModOn(::time(0));
	IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
	bot->reloadIpLisp(theClient, IpLisp);
	if (!IpLisp->updateData())
		{
		bot->Notice(theClient, "SQL insertion failed.");
		}
	else
		bot->Notice(theClient,"G-lines on %s %sactivated.%s", IpLisp->getName().c_str(), res ? "de" : "", res ? " Limits will keep being enforced via iauth." : "");
	}
else if(!strcasecmp(st[1].c_str(),"chemail"))
	{
	if(st.size() < 4)
		{
		bot->Notice(theClient,"SYNTAX: CHEMAIL <isp> <new email>");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	IpLisp = bot->getIpLisp(st[2]);

	if (IpLisp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	IpLisp->setEmail(st[3]);
	IpLisp->setModOn(::time(0));
	IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
	bot->reloadIpLisp(theClient, IpLisp);
	if (!IpLisp->updateData())
		{
		bot->Notice(theClient, "SQL insertion failed.");
		}
	else
		bot->Notice(theClient,"Email for %s set to: %s", IpLisp->getName().c_str(), st[3].c_str());
	}
else if(!strcasecmp(st[1].c_str(),"group"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: GROUP <isp> <yes|no>");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	IpLisp = bot->getIpLisp(st[2]);
	
	if (IpLisp == 0)
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
	IpLisp->setGroup(res);
	IpLisp->setModOn(::time(0));
	IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
	bot->reloadIpLisp(theClient, IpLisp);
	if (!IpLisp->updateData())
		{
		bot->Notice(theClient, "SQL insertion failed.");
		}
	else
		bot->Notice(theClient,"group for %s is now %sabled", IpLisp->getName().c_str(), res ? "en" : "dis");
	}
else if(!strcasecmp(st[1].c_str(),"glunidented"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: GLUNIDENTED <isp> <yes|no>");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	IpLisp = bot->getIpLisp(st[2]);
	
	if (IpLisp == 0)
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
	IpLisp->setGlunidented(res);
	IpLisp->setModOn(::time(0));
	IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
	bot->reloadIpLisp(theClient, IpLisp);
	if (!IpLisp->updateData())
		{
		bot->Notice(theClient, "SQL insertion failed.");
		}
	else
		bot->Notice(theClient,"glunidented for %s is now %sabled", IpLisp->getName().c_str(), res ? "en" : "dis");
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
	IpLisp = bot->getIpLisp(st[2]);
	
	if (IpLisp == 0)
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
	IpLisp->setForcecount(res);
	IpLisp->setModOn(::time(0));
	IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
	bot->reloadIpLisp(theClient, IpLisp);
	if (!IpLisp->updateData())
		{
		bot->Notice(theClient, "SQL insertion failed.");
		}
	else
		bot->Notice(theClient,"forcecount for %s is now %sabled", IpLisp->getName().c_str(), res ? "en" : "dis");
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
	IpLisp = bot->getIpLisp(st[2]);
	
	if (IpLisp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	else
		{
		if (!bot->ipLcidrChangeCheck(theClient, IpLisp, atoi(st[3]))) 
			return true;
		IpLisp->setCloneCidr(atoi(st[3].c_str()));
		IpLisp->setModOn(::time(0));
		IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
		bot->reloadIpLisp(theClient, IpLisp);
		if (!IpLisp->updateData())
			{
			bot->Notice(theClient, "SQL insertion failed.");
			}
		else
			bot->Notice(theClient,"%s is now checking clones on /%ds. Limit: %d", IpLisp->getName().c_str(), atoi(st[3].c_str()), IpLisp->getLimit());
		}
	}
else if(!strcasecmp(st[1].c_str(),"chilimit"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: CHILIMIT <isp> <ident limit>");
		return true;
		}
	if(atoi(st[3].c_str()) < 1) 
		{
		bot->Notice(theClient,"The limit must be > 0");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	IpLisp = bot->getIpLisp(st[2]);
	
	if (IpLisp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	else
		{
		IpLisp->setIdentLimit(atoi(st[3].c_str()));
		IpLisp->setModOn(::time(0));
		IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
		if (!IpLisp->updateData())
			{
			bot->Notice(theClient, "SQL insertion failed.");
			}
		else
			bot->Notice(theClient,"New Ident limit for %s is now %d", IpLisp->getName().c_str(), IpLisp->getIdentLimit());
		}
	}
else if(!strcasecmp(st[1].c_str(),"chlimit"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: CHLIMIT <isp> <limit>");
		return true;
		}
	if(atoi(st[3].c_str()) < 1) 
		{
		bot->Notice(theClient,"The limit must be > 0");
		return true;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Isp can't exceed 32 characters");
		return true;
		}
	IpLisp = bot->getIpLisp(st[2]);
	
	if (IpLisp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	else
		{
		IpLisp->setLimit(atoi(st[3].c_str()));
		IpLisp->setModOn(::time(0));
		IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
		if (!IpLisp->updateData())
			{
			bot->Notice(theClient, "SQL insertion failed.");
			}
		else
			bot->Notice(theClient,"New limit for %s is now %d", IpLisp->getName().c_str(), IpLisp->getLimit());
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
	IpLisp = bot->getIpLisp(st[3]);
	
	if (IpLisp != 0)
		{
		bot->Notice(theClient,"Isp %s already exists.", IpLisp->getName().c_str());
		return true;
		}
	IpLisp = bot->getIpLisp(st[2]);
	
	if (IpLisp == 0)
		{
		bot->Notice(theClient,"Isp not found.");
		return true;
		}
	else
		{
		IpLisp->setName(ccontrol::removeSqlChars(st[3]));
		IpLisp->setModOn(::time(0));
		IpLisp->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
		if (!IpLisp->updateData())
			{
			bot->Notice(theClient, "SQL insertion failed.");
			}
		else
			bot->Notice(theClient,"Renamed %s to %s", st[2].c_str(), IpLisp->getName().c_str());
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
		if(bot->delIpLisp(theClient,st[2]))
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

	if(bot->clearIsps(theClient))
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
	
//ipLuserInfo(

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
	return bot->ipLuserInfo(theClient, Target);
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
		if(bot->delIpLnb(theClient,st[2],st[3], false))
			{
			bot->Notice(theClient,"Successfully deleted netblock '%s'",st[3].c_str());
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


