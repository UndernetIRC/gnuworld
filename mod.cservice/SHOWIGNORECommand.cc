/* SHOWIGNORECommand.cc */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"responses.h"

const char SHOWIGNORECommand_cc_rcsId[] = "$Id: SHOWIGNORECommand.cc,v 1.12 2001/09/05 03:47:56 gte Exp $" ;

namespace gnuworld
{
using std::string ;

bool SHOWIGNORECommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SHOWIGNORE");

StringTokenizer st( Message ) ;
if( st.size() < 1 )
	{
	Usage(theClient);
	return true;
	}

size_t count = 0;

sqlUser* theUser = bot->isAuthed(theClient, false);

// TODO: Violation of encapsulation
if( bot->silenceList.empty() )
	{
	bot->Notice(theClient, bot->getResponse(theUser,
		language::ignore_list_empty,
		string("Ignore list is empty")));
	return true ;
	}

bot->Notice(theClient,
	bot->getResponse(theUser, language::ignore_list_start,
	string("Ignore list:")));

// TODO: Same as above
for( cservice::silenceListType::const_iterator ptr = bot->silenceList.begin() ;
	ptr != bot->silenceList.end() ; ++ptr )
	{
	bot->Notice(theClient, bot->getResponse(theUser,
		language::rpl_ignorelist, "%s for %i minutes").c_str(),
		ptr->first.c_str(),
		(((ptr->second.first - bot->currentTime()) / 60) % 60));
	count++;
	}

bot->Notice(theClient,
	bot->getResponse(theUser, language::ignore_list_end,
	string("-- End of Ignore List")));

return true ;
}

} // namespace gnuworld.
