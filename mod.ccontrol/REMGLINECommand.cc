/*
 * REMGLINECommand.cc
 *
 * Removes a gline
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char REMGLINECommand_cc_rcsId[] = "$Id: REMGLINECommand.cc,v 1.15 2001/12/23 09:07:57 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// remgline user@host
bool REMGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}
ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : REMGLINE %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());
if(st[1].substr(0,1) == "#")
	{
	bot->Notice(theClient,"Please user REMGCHAN to remove a BADCHAN gline");
	return false;
	}
ccGline *tmpGline = bot->findGline(st[1]);
if(tmpGline != NULL)
	{
	if(!tmpGline->Delete())
		bot->MsgChanLog("Error while removing gline for host %s from the db\n",st[1].c_str());
	bot->remGline(tmpGline);
	delete tmpGline;
	}	
if( !server->removeGline( st [ 1 ] ) )
	{
	// Remove failed
	bot->Notice( theClient, "Removal of gline failed\n" ) ;
	}
else
	{
	// Removal succeeded
	bot->Notice( theClient, "Removal of gline succeeded\n" ) ;
	bot->MsgChanLog( "[REMGLINE]: %s by : %s\n", st[ 1 ].c_str(),theClient->getNickName().c_str() ) ;
	}

return true ;
}

}
} // namespace gnuworld

