#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"


const char HELPCommand_cc_rcsId[] = "$Id $";

namespace gnuworld
{

using std::string ;


// help [command]
bool HELPCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

string banner = "--- Help Menu for " ;
banner += bot->getNickName() + " ---" ;

bot->Notice( theClient, banner ) ;


if( 1 == st.size() )
	{
	// Spit out all commands
	for( ccontrol::constCommandIterator ptr = bot->command_begin() ;
		ptr != bot->command_end() ; ++ptr )
		{
		bot->Notice( theClient, ptr->second->getName() ) ;
		}
	}
else
	{
	ccontrol::constCommandIterator ptr =
		bot->findCommand( string_upper( st[ 1 ] ) ) ;
	if( ptr == bot->command_end() )
		{
		bot->Notice( theClient, "Command not found" ) ;
		}
	else
		{
		//bot->GetHelp(theClient,st[1]);
		bot->Notice( theClient, ptr->second->getInfo() ) ;
		}
	}

return true ;
}
}
