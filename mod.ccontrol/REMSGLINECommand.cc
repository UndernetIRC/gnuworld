/*
 * REMSGLINECommand.cc
 *
 * Removes a gline
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char REMSGLINECommand_cc_rcsId[] = "$Id: REMSGLINECommand.cc,v 1.3 2003/05/19 22:19:45 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// remgline user@host
bool REMSGLINECommand::Exec( iClient* theClient, const string& Message )
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
bot->MsgChanLog("REMSGLINE %s\n",st.assemble(1).c_str());

if(st[1].substr(0,1) == "#")
	{
	bot->Notice(theClient,"Please use REMGCHAN to remove a BADCHAN gline");
	return false;
	}
ccGline *tmpGline;
bool realName = false;
if(st[1].substr(0,1) != "$")
    tmpGline = bot->findGline(st[1]);
else
	{    
	tmpGline = bot->findRealGline(st[1]);
	realName = true;
	}
if(tmpGline != NULL)
	{
	if(!tmpGline->Delete())
		bot->MsgChanLog("Error while removing gline for host %s from the db\n",st[1].c_str());
	bot->remGline(tmpGline);
	delete tmpGline;
	}	
if(!realName)
	server->removeGline(st[1],bot);
bot->Notice( theClient, "Removal of gline(%s) succeeded\n",st[1].c_str() ) ;
return true ;
}

}
} // namespace gnuworld

