/*
 * SCANGLINECommand.cc
 *
 * Check if a certain host is glined, if it is returns the glines info
 *
 */

#include	<string>
#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char SCANGLINECommand_cc_rcsId[] = "$Id: SCANGLINECommand.cc,v 1.8 2002/01/25 14:11:31 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SCANGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : SCANGLINE %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());

bot->listGlines(theClient,st[1]);

/*vector< const Gline* > glines = server->matchGline( st[ 1 ] ) ;

bot->Notice( theClient, "Found %d matches", glines.size() ) ;
if( glines.empty() )
	{
	return true ;
	}

bot->Notice( theClient, "Current time: %d", ::time( 0 ) ) ;

for( vector< const Gline* >::const_iterator ptr = glines.begin() ;
	ptr != glines.end() ; ++ptr )
	{
	bot->Notice( theClient, "%s expires at %d, set by %s because %s",
		(*ptr)->getUserHost().c_str(),
		(*ptr)->getExpiration(),
		(*ptr)->getSetBy().c_str(),
		(*ptr)->getReason().c_str() ) ;
	}*/

return true ;
}

}
} // namespace gnuworld

