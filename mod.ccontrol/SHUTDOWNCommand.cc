/*
 * SHUTDOWNCommand.cc
 *
 * Shuts down the bot
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"server.h"
#include	<string.h>

const char SHUTDOWNCommand_cc_rcsId[] = "$Id: SHUTDOWNCommand.cc,v 1.1 2001/11/21 20:54:40 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SHUTDOWNCommand::Exec( iClient* theClient, const string& Message )
{	 
StringTokenizer st( Message ) ;
	
if(st.size() < 2) 
	{
	Usage(theClient);
	return true;
	}
char sq[512];
sprintf(sq,"%s SQ %s 0 :(%s)%s\n",
	server->getCharYY()
//	,Network->findServer(server->getUplinkCharYY())->getName().c_str()
	,server->getName().c_str()
	,theClient->getNickName().c_str(),st.assemble(1).c_str());
bot->Write(bot->getCharYYXXX() + " Q :" +st.assemble(1) + "\n");
bot->Write(sq);

return true;
}

}
} // namespace gnuworld

