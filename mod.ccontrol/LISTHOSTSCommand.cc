/*
 * LISTHOSTSCommand.cc
 *
 * Get an oper hosts entry
 *
 */

#include	<string>
#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char LISTHOSTSCommand_cc_rcsId[] = "$Id: LISTHOSTSCommand.cc,v 1.8 2001/12/23 09:07:57 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool LISTHOSTSCommand::Exec( iClient* theClient, const string& Message)
{	 
StringTokenizer st( Message ) ;


if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }
	
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}
ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : LISTHOSTS %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());

//Fetch the oper data base entry
//ccUser *tmpUser = bot->GetUser(st[1]);
tmpUser = bot->GetOper(bot->removeSqlChars(st[1]));

if(!tmpUser)
	{
        bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
        return false;
	}

if(bot->listHosts(tmpUser,theClient))
	{
	bot->Notice(theClient,"End of hosts for user %s",st[1].c_str());
	}
else
	{
	bot->Notice(theClient,"Error while accessing %s hostlist",st[1].c_str());
	}

return true;
}

}
} // namespace gnuworld

