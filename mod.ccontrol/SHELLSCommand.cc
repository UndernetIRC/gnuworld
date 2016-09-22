/**
 * SHELLSCommand.cc
 * Modify the shells exceptions
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
 * $Id: SHELLSCommand.cc,v 1.3 2008/12/28 12:21:15 hidden1 Exp $
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

RCSTAG( "$Id: SHELLSCommand.cc,v 1.3 2008/12/28 12:21:15 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SHELLSCommand::Exec( iClient* theClient, const string& Message )
{	 
StringTokenizer st( Message ) ;
ccShellco* Shellco;
ccShellnb* Shellnb;
	
if(st.size() < 2) 
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("SHELLS %s\n",st.assemble(1).c_str());
if(!strcasecmp(st[1].c_str(),"list")) //Trying to list all exceptions?
	{
	bot->listShellExceptions(theClient);
	return true;
	}
else if(!strcasecmp(st[1].c_str(),"addcompany"))
	{
	if(st.size() != 4) 
		{
		bot->Notice(theClient,"SYNTAX: ADDCOMPANY <name> <max connections>");
		return false;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Company name cannot exceeed 32 characters");
		return false;
		}
	if (atoi(st[3].c_str()) == 0)
		{
		bot->Notice(theClient,"SYNTAX: ADDCOMPANY <name> <max connections>");
		return false;
		}

	Shellco = bot->getShellco(st[2]);
	
	if (Shellco != 0)
		{
		bot->Notice(theClient,"A shell company already exists with that name.");
		return false;
		}

	else
		{
		if(bot->insertShellco(theClient,st[2],atoi(st[3].c_str())))
			{
			bot->Notice(theClient,"Successfully added shell company '%s' for %d connections"
				    ,st[2].c_str(),atoi(st[3].c_str()));
			}
		}
	}
else if(!strcasecmp(st[1].c_str(),"addnetblock"))
	{
	if(st.size() != 4) 
		{
		bot->Notice(theClient,"SYNTAX: ADDNETBLOCK <Shell company> <netblock>");
		return false;
		}
	if(st[3].size() > 18)
		{
		bot->Notice(theClient,"Suggested netblock format: x.x.x.x/xx");
		return false;
		}

	Shellnb = bot->getShellnb(st[3]);
	if (Shellnb != 0)
		{
		bot->Notice(theClient,"%s already belongs to company %s", st[3].c_str(), Shellnb->shellco->getName().c_str());
		return false;
		}
	
	Shellco = bot->getShellco(st[2]);
	
	if (Shellco == 0)
		{
		bot->Notice(theClient,"Shell company not found. Use ADDCOMPANY first");
		return false;
		}

	else
		{
		if(bot->insertShellnb(theClient,st[3],Shellco->getID()))
			{
			bot->Notice(theClient,"Successfully added '%s' to '%s'"
				    ,st[3].c_str(),st[2].c_str());
			}
		}
	}
else if(!strcasecmp(st[1].c_str(),"chlimit"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: CHLIMIT <company> <limit>");
		return false;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Company can't exceed 32 characters");
		return false;
		}
	Shellco = bot->getShellco(st[2]);
	
	if (Shellco == 0)
		{
		bot->Notice(theClient,"Shell company not found.");
		return false;
		}
	else
		{
		Shellco->setLimit(atoi(st[3].c_str()));
		Shellco->setModOn(::time(0));
		Shellco->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
		if (!Shellco->updateData()) 
			{
			bot->Notice(theClient, "SQL insertion failed.");
			}
		else
			bot->Notice(theClient,"New limit for %s is now %d", Shellco->getName().c_str(), Shellco->getLimit());
		}
	}
else if(!strcasecmp(st[1].c_str(),"chname"))
	{
	if(st.size() < 4) 
		{
		bot->Notice(theClient,"SYNTAX: CHNAME <company> <new name>");
		return false;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Company can't be more than 32 characters");
		return false;
		}
	Shellco = bot->getShellco(st[3]);
	
	if (Shellco != 0)
		{
		bot->Notice(theClient,"Shell company %s already exists.", Shellco->getName().c_str());
		return false;
		}
	Shellco = bot->getShellco(st[2]);
	
	if (Shellco == 0)
		{
		bot->Notice(theClient,"Shell company not found.");
		return false;
		}
	else
		{
		Shellco->setName(ccontrol::removeSqlChars(st[3]));
		Shellco->setModOn(::time(0));
		Shellco->setModBy(ccontrol::removeSqlChars(theClient->getRealNickUserHost()));
		if (!Shellco->updateData()) 
			{
			bot->Notice(theClient, "SQL insertion failed.");
			}
		else
			bot->Notice(theClient,"Renamed %s to %s", st[2].c_str(), Shellco->getName().c_str());
		}
	}
else if(!strcasecmp(st[1].c_str(),"delcompany"))
	{
	if(st.size() < 3) 
		{
		bot->Notice(theClient,"You must specify the company you want to delete");
		return false;
		}
	if(st[2].size() > 32)
		{
		bot->Notice(theClient,"Company can't exceed 32 characters");
		return false;
		}
	else
		{
		if(bot->delShellco(theClient,st[2]))
			{
			bot->Notice(theClient,"Successfully deleted company '%s'",st[2].c_str());
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
		bot->Notice(theClient,"Successfully deleted all shell companies");
		}
	}

else if(!strcasecmp(st[1].c_str(),"test"))
	{
	if (st.size() > 2)
		{
		bot->test(theClient, st[2]);
		}
	return true;

	}
	

else if(!strcasecmp(st[1].c_str(),"delnetblock"))
	{
	if(st.size() < 3) 
		{
		bot->Notice(theClient,"You must specify the netblock you want to delete");
		return false;
		}
	if(st[2].size() > 18)
		{
		bot->Notice(theClient,"A netblock can't exceed 18 characters");
		return false;
		}
	else
		{
		if(bot->delShellnb(theClient,st[2]))
			{
			bot->Notice(theClient,"Successfully deleted netblock '%s'",st[2].c_str());
			}
		}

	} else {
		/* unknown command */
		Usage(theClient);
		return false;
	}
return true;
}

}
} // namespace gnuworld

