/*
 * REMGCHANCommand.cc
 *
 * Removes a BADCHAN gline
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"

const char REMGCHANCommand_cc_rcsId[] = "$Id: REMGCHANCommand.cc,v 1.9 2002/03/01 21:15:55 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// remgline user@host
bool REMGCHANCommand::Exec( iClient* theClient, const string& Message )
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

if((st[1].substr(0,1) != "#") || (st[1].size() > channel::MaxName))
	{
	bot->Notice(theClient,"Invalid channel name, must begin with # and cant be more than %d chars"
			,channel::MaxName);
	return false;
	}
bot->MsgChanLog("REMGCHAN %s\n",st.assemble(1).c_str());

ccGline *tmpGline = bot->findGline(bot->removeSqlChars(st[1]));
if(tmpGline != NULL)
	{
	if(!tmpGline->Delete())
		bot->MsgChanLog("Error while removing gline for channel %s from the db\n",st[1].c_str());
	bot->remGline(tmpGline);
	delete tmpGline;
	}	
//bot->setRemoving(st[1]);
server->removeGline( st [ 1 ] ,bot);
//bot->unSetRemoving();
return true ;
}

}
} // namespace gnuworld

