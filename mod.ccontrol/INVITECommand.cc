/*
 * INVITECommand.cc
 *
 * Cause the bot to invite you to a channel
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"Network.h"

const char INVITECommand_cc_rcsId[] = "$Id: INVITECommand.cc,v 1.12 2002/01/17 20:04:05 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// invite #channel
bool INVITECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;

if( st.size() == 1 )
	{
	// send help
	Usage( theClient ) ;
	return false ;
	}
if(st[1].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d chars",channel::MaxName);
	return false;
	}
ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : INVITE %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());
else
        bot->MsgChanLog("(Unknown) - %s : INVITE %s\n"
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());

//If the channel doesnt begin with # add it 
string chanName = st[ 1 ] ;
if( chanName[ 0 ] != '#' )
	{
	chanName.insert( chanName.begin(), '#' ) ;
	}
string invNum;
if(st.size() > 2)
	{
	iClient* tmpClient = Network->findNick(st[2]);
	if(tmpClient == NULL)
		{
		bot->Notice(theClient,"I cant find %s anywere",st[2].c_str());
		return true;
		}
	if((!tmpUser) && (tmpClient != theClient))
		{
		bot->Notice(theClient,"You must login to invite someone else!");
		return true;
		}

	invNum = tmpClient->getNickName();
	}
else
	{
	invNum = theClient->getNickName();
	}
char buf[ 512 ] = { 0 } ;

// Invite buffer
sprintf( buf, "%s I %s :%s\n",
	bot->getCharYYXXX().c_str(),
	invNum.c_str(),
	chanName.c_str() ) ;

if( bot->isOperChan( chanName ) )
	{
	// No problem

	// Send the invitation
	bot->QuoteAsServer( buf ) ;
	return true ;
	}

// Else, this is a user channel
// Note that this assumes that the bot
// is in no channels other than oper
// channels.

// Join
bot->Join( chanName, string(), 0, false ) ;

// Invite
bot->QuoteAsServer( buf ) ;

// Part
bot->Part( chanName ) ;

// Wallops
if( !bot->isOperChan( chanName ) )
	{
	bot->MsgChanLog("%s is cordially invited to channel %s\n",theClient->getNickName().c_str(),chanName.c_str());
	}

return true ;
}

}
}

