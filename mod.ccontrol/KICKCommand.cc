/*
 * KICKCommand.cc
 *
 * Kick a user from a channel
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"

const char KICKCommand_cc_rcsId[] = "$Id: KICKCommand.cc,v 1.8 2001/12/23 09:07:57 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// kick #channel nick reason
bool KICKCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 4 )
	{
	Usage( theClient ) ;
	return true ;
	}

//Check if the channel begins with # , if not add it
string chanName = st[ 1 ] ;
if( chanName[ 0 ] != '#' )
	{
	chanName.insert( chanName.begin(), '#' ) ;
	}

if(chanName.size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d chars",channel::MaxName);
	}
ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : KICK %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel: %s",
		chanName.c_str() ) ;
	return true ;
	}

iClient* Target = Network->findNick( st[ 2 ] ) ;
if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find nick: %s",
		st[ 2 ].c_str() ) ;
	return true ;
	}

if( NULL == theChan->findUser( Target ) )
	{
	bot->Notice( theClient, "User %s was not found "
	"on channel %s",
	st[ 1 ].c_str(),
	theChan->getName().c_str() ) ;
	return true ;
	}

if( Target->getMode(iClient::MODE_SERVICES))
	{
	bot->Notice(theClient,"Are you trying to get me introuble with %s?",st[ 2 ].c_str());
	return false;
	}
bot->Notice( theClient, "Kicking %s from channel %s because %s",
	Target->getNickName().c_str(),
	chanName.c_str(),
	st.assemble( 3 ).c_str() ) ;

//If its an oper channel , no need to join and part the channel , so just kick the user
if( bot->isOperChan( chanName ) )
	{
	bot->Kick( theChan, Target, st.assemble( 3 ) ) ;
	return true ;
	}
//Its not an oper channel 
bot->Join( chanName, string(), 0, true ) ;

bot->Kick( theChan, Target, st.assemble( 3 ) ) ;

bot->Part( chanName ) ;

return true ;
}

}
}
