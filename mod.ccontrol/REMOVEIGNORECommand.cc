/*
 * REMOVEIGNORECommand.cc
 *
 * Removes ignored host
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include        "ccLogin.h"

const char REMOVEIGNORECommand_cc_rcsId[] = "$Id: REMOVEIGNORECommand.cc,v 1.4 2001/12/23 09:07:57 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

// remoperchan #channel

namespace uworld
{

bool REMOVEIGNORECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}
ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : REMIGNORE %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());
string::size_type atPos = st[ 1 ].find_first_of( '@' ) ;
if( string::npos == atPos )
	{
	iClient *igClient = Network->findNick(st[1]);
	if(!igClient)
		{
		bot->Notice(theClient,"There is no such user as %s",st[1].c_str());
		return false;
		}
	if(bot->removeIgnore(igClient) == IGNORE_REMOVED)
		{
		bot->Notice(theClient,"Successfully removed ignore for user %s",st[1].c_str());
		bot->MsgChanLog("Removed ignore for %s by %s request\n",st[1].c_str(),theClient->getNickName().c_str());
		}

	else
		bot->Notice(theClient,"Cant find ignore for user %s",st[1].c_str());
	}
else
	if(bot->removeIgnore(st[1]) == IGNORE_REMOVED)
		{	
		bot->Notice(theClient,"Successfully removed ignore for host %s",st[1].c_str());
		bot->MsgChanLog("Removed ignore for %s by %s request\n",st[1].c_str(),theClient->getNickName().c_str());
		}
	else
		bot->Notice(theClient,"Cant find ignore for host %s",st[1].c_str());

return true;
}

}
} // namespace gnuworld

