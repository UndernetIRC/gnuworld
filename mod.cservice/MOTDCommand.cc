/* MOTDCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include 	"responses.h"

const char MOTDCommand_cc_rcsId[] = "$Id: MOTDCommand.cc,v 1.3 2001/02/16 20:20:26 plexus Exp $" ;

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

	bot->Notice(theClient, "There is no motd (or spoon for that matter)");
         
	return true ;
} 

} // namespace gnuworld.
