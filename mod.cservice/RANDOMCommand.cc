/* RANDOMCommand.cc */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"Network.h"
#include	"cservice.h"

const char RANDOMCommand_cc_rcsId[] = "$Id: RANDOMCommand.cc,v 1.5 2002/03/04 22:53:50 gte Exp $" ;

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
