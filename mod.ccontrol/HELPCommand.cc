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


const char HELPCommand_cc_rcsId[] = "$Id: HELPCommand.cc,v 1.15 2002/01/05 14:02:59 isomer Exp $";

namespace gnuworld
{

using std::string ;


// help [command]
namespace uworld
{

bool HELPCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

ccUser *tmpAuth = bot->IsAuth(theClient);
if(!tmpAuth)
	return false;
string banner = "--- Help Menu for " ;
banner += bot->getNickName() + " --- (Showing commands which are available for you)" ;

bot->Notice( theClient, "%s", banner ) ;

int ComLevel;
// Check if the user didnt supply a command 
if( 1 == st.size() )
	{
	// Spit out all commands
	string Show;
	int Num = 0;
	for( ccontrol::constCommandIterator ptr = bot->command_begin() ;
		ptr != bot->command_end() ; ++ptr )
		{
		ComLevel = ptr->second->getFlags();
		//ComLevel &= ~flg_NOLOG; 
		if((ComLevel == 0) || (tmpAuth->gotAccess(ptr->second)) )
			{
			Show += (ptr->second->getName() + " ");
			++Num;
			if(Show.size() > 80)
				{
				bot->Notice( theClient, "%s", Show) ;
				Num = 0;
				Show.assign("");
				}
			}
		}
	if(!Show.empty())
		bot->Notice( theClient, "%s", Show) ;
	bot->Notice(theClient,"End of command list");
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
			bot->GetHelp(theClient,st[1]);
		}
	}

return true ;
}

}
}
