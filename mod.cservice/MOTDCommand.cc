/* MOTDCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include 	"responses.h"

const char MOTDCommand_cc_rcsId[] = "$Id: MOTDCommand.cc,v 1.2 2001/02/12 05:42:55 isomer Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool MOTDCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() != 1 )
	{
		Usage(theClient);
		return true;
	}

	bot->Notice(theClient, 
		bot->getResponse(
			bot->isAuthed(theClient, false),
			language::motd,
			"There is no motd (or spoon for that matter)"
		)
	);
         
	return true ;
} 

} // namespace gnuworld.
