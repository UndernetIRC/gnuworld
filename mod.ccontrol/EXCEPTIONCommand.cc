/*
 * EXCEPTIONCommand.cc
 *
 * Modify the exceptions list
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include 	"Constants.h"

const char EXCEPTIONCommand_cc_rcsId[] = "$Id: EXCEPTIONCommand.cc,v 1.13 2003/04/28 09:44:20 mrbean_ Exp $";

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

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
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
		bot->Notice(theClient,"you must specify the host you want to add the connection count and the reason\n");
		return false;
		}
	if(st[2].size() > 128)
	{
		bot->Notice(theClient,"Hostname can't be more than 128 chars");
		return false;
	}
	else if(st[4].size() > exceptions::MAX_REASON)
	{
		bot->Notice(theClient,"Reason can't be more than %d chars",exceptions::MAX_REASON);
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
			bot->Notice(theClient,"Successfully added an exception on host %s for %d connections"
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
		bot->Notice(theClient,"Hostname can't be more than 128 chars");
		return false;
		}
	else
		{
		if(!bot->delException(theClient,st[2]))
			{
			bot->Notice(theClient,"Error while deleting exception for host %s",st[2].c_str());
			}
		else
			{
			bot->Notice(theClient,"Successfully deleted exception for host %s",st[2].c_str());
			}
		
		}

	}	
return true;
}

}
} // namespace gnuworld

