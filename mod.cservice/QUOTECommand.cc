/* QUOTECommand.cc */

#include	<string>
 
#include	"cservice.h"
//#include	"sqlUser.h"
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"levels.h" 
#include	"responses.h"

const char QUOTECommand_cc_rcsId[] = "$Id: QUOTECommand.cc,v 1.4 2001/02/21 00:14:43 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
 
bool QUOTECommand::Exec( iClient* theClient, const string& Message )
{ 
StringTokenizer st( Message ) ;
if( st.size() < 1 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false; 
 	}

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::quote)
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
	return false;
	} 
 
bot->Write( st.assemble(1) ); 

return true ;
} 

} // namespace gnuworld.
