/* REHASHCommand.cc */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char REHASHCommand_cc_rcsId[] = "$Id: REHASHCommand.cc,v 1.4 2001/10/19 00:11:20 gte Exp $" ;

namespace gnuworld
{
using std::string ;

bool REHASHCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.REHASH");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser) return false;

int level = bot->getAdminAccessLevel(theUser);
if (level < level::rehash)
{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("You have insufficient access to perform that command.")));
	return false;
}

string option = string_upper(st[1]);

if (option == "TRANSLATIONS")
	{
		bot->languageTable.clear();
		bot->translationTable.clear();
		bot->loadTranslationTable();
		bot->Notice(theClient, "Done. %i entries in language table.",
			bot->translationTable.size());
	}

if (option == "HELP")
	{
		bot->helpTable.clear();
		bot->loadHelpTable();
		bot->Notice(theClient, "Done. %i entries in help table.",
			bot->helpTable.size());
	}

return true ;
}

} // namespace gnuworld.
