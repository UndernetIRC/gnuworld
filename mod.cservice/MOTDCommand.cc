/* MOTDCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include 	"responses.h"

const char MOTDCommand_cc_rcsId[] = "$Id: MOTDCommand.cc,v 1.6 2001/04/13 19:12:14 gte Exp $" ;

namespace gnuworld
{

using std::string ;
 
bool MOTDCommand::Exec( iClient* theClient, const string& Message )
{ 
StringTokenizer st( Message ) ;
if( st.size() != 1 )
	{
	Usage(theClient);
	return true;
	}

	sqlUser* theUser = bot->isAuthed(theClient, false);

	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::motd,
			string("No MOTD set.")));
         
return true ;
} 

} // namespace gnuworld.
