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

const char REMGLINECommand_cc_rcsId[] = "$Id: REMGLINECommand.cc,v 1.20 2002/03/01 21:15:55 mrbean_ Exp $";

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
bot->MsgChanLog("REMGLINE %s\n",st.assemble(1).c_str());

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
server->removeGline(st[1],bot);
bot->Notice( theClient, "Removal of gline succeeded\n" ) ;
return true ;
}

}
} // namespace gnuworld

