#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

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
	bot->Wallops( wallopMe ) ;
	}

return true ;
}
}

