/* MOTDCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include 	"responses.h"

const char MOTDCommand_cc_rcsId[] = "$Id: MOTDCommand.cc,v 1.5 2001/03/08 23:49:01 gte Exp $" ;

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

bot->Notice(theClient, "How are you gentlemen! All your base are belong to us.");
         
return true ;
} 

} // namespace gnuworld.
