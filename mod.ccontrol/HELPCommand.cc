/*
 * HELPCommand.cc
 *
 * Shows help text on commands
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"


const char HELPCommand_cc_rcsId[] = "$Id: HELPCommand.cc,v 1.10 2001/07/26 20:12:40 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


// help [command]
namespace uworld
{

bool HELPCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

AuthInfo *tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
if(!tmpAuth)
	return false;
string banner = "--- Help Menu for " ;
banner += bot->getNickName() + " --- (Showing commands which are available for you)" ;

bot->Notice( theClient, banner ) ;

int ComLevel;
// Check if the user didnt supply a command 
if( 1 == st.size() )
	{
	// Spit out all commands
	for( ccontrol::constCommandIterator ptr = bot->command_begin() ;
		ptr != bot->command_end() ; ++ptr )
		{
		ComLevel = ptr->second->getFlags();
		//ComLevel &= ~flg_NOLOG; 
		if((ComLevel == 0) || (ComLevel & tmpAuth->getAccess()) )
			bot->Notice( theClient, ptr->second->getName() ) ;
		}
	}
else //Supplied a command, show only the help for that command (if it exists)
	{
	ccontrol::constCommandIterator ptr =
		bot->findCommand( string_upper( st[ 1 ] ) ) ;
	if( ptr == bot->command_end() )
		{
		bot->Notice( theClient, "Command not found" ) ;
		}
	else
		{
		if(2 == st.size())
			bot->GetHelp(theClient,st[1]);
		else
			bot->GetHelp(theClient,st[1],st[2]);
		}
	}

return true ;
}

}
}
