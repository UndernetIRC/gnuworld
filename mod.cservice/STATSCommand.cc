/* STATSCommand.cc */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"

const char STATSCommand_cc_rcsId[] = "$Id: STATSCommand.cc,v 1.2 2001/09/05 03:47:56 gte Exp $" ;

namespace gnuworld
{
using std::string ;

bool STATSCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.STATS");

sqlUser* theUser = bot->isAuthed(theClient, false);
if (!theUser)
	{
	return false;
	}

int admLevel = bot->getAdminAccessLevel(theUser);
if (!admLevel) return false;

bot->Notice(theClient, "CMaster Command/SQL Query Statistics:");
for( cservice::statsMapType::iterator ptr = bot->statsMap.begin() ;
	ptr != bot->statsMap.end() ; ++ptr )
	{
	bot->Notice(theClient, "%s: %i", ptr->first.c_str(), ptr->second);
	}

bot->Notice(theClient, "-- End of STATS");

return true ;
}

} // namespace gnuworld.
