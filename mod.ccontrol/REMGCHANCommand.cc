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

const char REMGCHANCommand_cc_rcsId[] = "$Id: REMGCHANCommand.cc,v 1.2 2001/11/08 23:13:29 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// remgline user@host
bool REMGCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}
if((st[1].substr(0,1) != "#") || (st[1].size() > 200))
	{
	bot->Notice(theClient,"Invalid channel name, must begin with # and cant be more than 200 chars");
	return false;
	}
ccGline *tmpGline = bot->findGline(bot->removeSqlChars(st[1]));
if(tmpGline != NULL)
	{
	if(!tmpGline->Delete())
		bot->MsgChanLog("Error while removing gline for channel %s from the db\n",st[1].c_str());
	bot->remGline(tmpGline);
	delete tmpGline;
	}	
server->removeGline( st [ 1 ] );
bot->MsgChanLog( "[REMGCHAN]: %s by : %s\n", st[ 1 ].c_str(),theClient->getNickName().c_str() ) ;
return true ;
}

}
} // namespace gnuworld

