/*
 * CHANINFOCommand.cc
 * 
 * Shows information about a channel
 *
 */
#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char CHANINFOCommand_cc_rcsId[] = "$Id: CHANINFOCommand.cc,v 1.5 2001/07/17 16:58:27 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;

bool CHANINFOCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s",
		st[ 1 ].c_str() ) ;
	return true ;
	}

bot->Notice( theClient, "Channel %s is mode %s",
	st[ 1 ].c_str(),
	theChan->getModeString().c_str() ) ;
bot->Notice( theClient, "Created at time: %d",
	theChan->getCreationTime() ) ;
bot->Notice( theClient, "Number of channel users: %d",
	theChan->size() ) ;

return true ;
}

} // namespace gnuworld
