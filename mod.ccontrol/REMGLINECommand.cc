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

const char REMGLINECommand_cc_rcsId[] = "$Id: REMGLINECommand.cc,v 1.11 2001/07/17 16:58:27 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;

// remgline user@host
bool REMGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
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
	bot->MsgChanLog( "RemGline: %s by : %s\n", st[ 1 ].c_str(),theClient->getNickName().c_str() ) ;
	}

return true ;
}

} // namespace gnuworld

