/**
 * EXCEPTIONCommand.cc
 * Modify the exceptions list
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
 * $Id: EXCEPTIONCommand.cc,v 1.18 2006/09/26 17:35:58 kewlio Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include 	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: EXCEPTIONCommand.cc,v 1.18 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool EXCEPTIONCommand::Exec( iClient* theClient, const string& Message )
{	 
StringTokenizer st( Message ) ;
	
if(st.size() < 2) 
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("EXCEPTIONS %s\n",st.assemble(1).c_str());
if(!strcasecmp(st[1].c_str(),"list")) //Trying to list all exceptions?
	{
	bot->listExceptions(theClient);
	return true;
	}
else if(!strcasecmp(st[1].c_str(),"add")) //Trying to add an exception?
	{
	if(st.size() < 5) 
		{
		bot->Notice(theClient,"you must specify the host you want to add, the connection count and the reason\n");
		return false;
		}
	if(st[2].size() > 128)
	{
		bot->Notice(theClient,"Hostname can't be more than 128 characters");
		return false;
	}
	else if(st[4].size() > exceptions::MAX_REASON)
	{
		bot->Notice(theClient,"Reason can't be more than %d characters",
			exceptions::MAX_REASON);
		return false;

	}
	else
		{
		if(!bot->insertException(theClient,st[2],atoi(st[3].c_str()),st.assemble(4)))
			{
			bot->Notice(theClient,"Error while adding exception\n");
			}
		else
			{
			bot->Notice(theClient,"Successfully added an exception on host '%s' for %d connections"
				    ,st[2].c_str(),atoi(st[3].c_str()));
			}
		}
	}
else if(!strcasecmp(st[1].c_str(),"del")) //Trying to add an exception?
	{
	if(st.size() < 3) 
		{
		bot->Notice(theClient,"you must specify the host you want to delete");
		return false;
		}
	if(st[2].size() > 128)
		{
		bot->Notice(theClient,"Hostname can't be more than 128 characters");
		return false;
		}
	else
		{
		if(!bot->delException(theClient,st[2]))
			{
			bot->Notice(theClient,"Error while deleting exception for host '%s'",st[2].c_str());
			}
		else
			{
			bot->Notice(theClient,"Successfully deleted exception for host '%s'",st[2].c_str());
			}
		
		}

	} else {
		/* unknown command */
		bot->Notice(theClient, "You must use 'add', 'del' or 'list'!");
		return false;
	}
return true;
}

}
} // namespace gnuworld

