/* ADDCOMMENTCommand.cc */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char ADDCOMMENTCommand_cc_rcsId[] = "$Id: ADDCOMMENTCommand.cc,v 1.2 2001/10/26 21:52:27 gte Exp $" ;

namespace gnuworld
{
using std::string ;

bool ADDCOMMENTCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.ADDCOMMENT");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, false);
if (!theUser)
	{
	return false;
	}

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::addcommentcmd) return false;

/*
 *  Check the person we're trying to add is actually registered.
 */

sqlUser* targetUser = bot->getUserRecord(st[1]);
if (!targetUser)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::not_registered).c_str(),
		st[1].c_str()
	);
	return false;
	}

/*
 * Add the comment.
 */

targetUser->writeEvent(sqlUser::EV_COMMENT, theUser, st.assemble(2));

bot->Notice(theClient, "Done. Added comment to %s", targetUser->getUserName().c_str());

return true ;
}

} // namespace gnuworld.
