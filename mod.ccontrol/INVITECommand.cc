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

const char INVITECommand_cc_rcsId[] = "$Id: INVITECommand.cc,v 1.5 2001/05/01 22:26:49 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

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

//If the channel doesnt begin with # add it 
string chanName = st[ 1 ] ;
if( chanName[ 0 ] != '#' )
	{
	chanName.insert( chanName.begin(), '#' ) ;
	}

char buf[ 512 ] = { 0 } ;

// Invite buffer
sprintf( buf, "%s I %s :%s\n",
	bot->getCharYYXXX().c_str(),
	theClient->getNickName().c_str(),
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
bot->Join( chanName, string(), 0, true ) ;

// Invite
bot->QuoteAsServer( buf ) ;

// Part
bot->Part( chanName ) ;

// Wallops
if( !bot->isOperChan( chanName ) )
	{
	string wallopMe = theClient->getCharYYXXX() ;
	wallopMe += " is cordially invited to channel "
		+ chanName ;
	wallopMe+='\n';
	bot->MsgChanLog(wallopMe.c_str());
	}

return true ;
}
}

