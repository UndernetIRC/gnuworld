/*
 * MODERATECommand.cc
 *
 * Cause the bot to moderate a  channel
 *
 */

#include	<string>
#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"

const char MODERATECommand_cc_rcsId[] = "$Id: MODERATECommand.cc,v 1.5 2001/07/30 16:58:39 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool MODERATECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[1].size() > 200)
	{
	bot->Notice(theClient,"Channel name can't be more than 200 chars");
	return false;
	}
Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}

if(theChan->getMode(Channel::MODE_M))
	{
	bot->Notice( theClient,"Channel %s is already moderated",st[ 1 ].c_str());
	return false;
	}

theChan->setMode(Channel::MODE_M);
bot->ModeAsServer( theChan, "+m");
return true;
}

}
} // namespace gnuworld
