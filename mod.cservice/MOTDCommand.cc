/* MOTDCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include 	"responses.h"

const char MOTDCommand_cc_rcsId[] = "$Id: MOTDCommand.cc,v 1.4 2001/02/21 00:14:43 dan_karrels Exp $" ;

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

bot->Notice(theClient, "There is no motd (or spoon for that matter)");
         
return true ;
} 

} // namespace gnuworld.
