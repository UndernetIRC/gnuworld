/*
 * UNMODERATECommand.cc
 *
 * Cause the bot to unmoderate a  channel
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"

const char UNMODERATECommand_cc_rcsId[] = "$Id: UNMODERATECommand.cc,v 1.1 2001/03/10 18:33:23 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


bool UNMODERATECommand::Exec( iClient* theClient, const string& Message )
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
	bot->Notice( theClient, "Unable to find channel %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}

if(!theChan->getMode(Channel::MODE_M))
	{
	bot->Notice( theClient,"Channel %s is not moderated",st[ 1 ].c_str());
	return false;
	}

theChan->removeMode(Channel::MODE_M);
bot->ModeAsServer( theChan, "-m");
}
}