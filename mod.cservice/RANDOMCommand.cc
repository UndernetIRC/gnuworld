/* RANDOMCommand.cc */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"

const char RANDOMCommand_cc_rcsId[] = "$Id: RANDOMCommand.cc,v 1.4 2001/09/26 01:10:31 gte Exp $" ;

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

sqlChannel* test = new sqlChannel(bot->SQLDb);
test->loadData("#coder-com");

return true ;
}

} // namespace gnuworld.
