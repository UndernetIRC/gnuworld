/* RANDOMCommand.cc */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"

const char RANDOMCommand_cc_rcsId[] = "$Id: RANDOMCommand.cc,v 1.3 2001/09/05 03:47:56 gte Exp $" ;

namespace gnuworld
{
using std::string ;

bool RANDOMCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.RANDOM");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

return true ;
}

} // namespace gnuworld.
