/* QUOTECommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 

const char QUOTECommand_cc_rcsId[] = "$Id: QUOTECommand.cc,v 1.1 2001/01/29 02:16:27 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool QUOTECommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 1 )
	{
		Usage(theClient);
		return true;
	}

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false; 
 
	int admLevel = bot->getAdminAccessLevel(theUser);
	if (admLevel < level::quote)
	{
		bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
		return false;
	} 
 
	bot->Write( st.assemble(1) ); 

	return true ;
} 

} // namespace gnuworld.
