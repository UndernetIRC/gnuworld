/*
 * SUSPENDMECommand.cc
 *
 * $Id: SUSPENDMECommand.cc,v 1.3 2003/01/08 23:23:36 gte Exp $
 */

#include	<string>
#include	<ctime>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char SUSPENDMECommand_cc_rcsId[] = "$Id: SUSPENDMECommand.cc,v 1.3 2003/01/08 23:23:36 gte Exp $" ;

namespace gnuworld
{
using std::string ;
using namespace level;

bool SUSPENDMECommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SUSPENDME");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false;
	}

/*
 * Check password, if its wrong, bye bye.
 */

if (!bot->isPasswordRight(theUser, st.assemble(1)))
	{
	bot->Notice(theClient, "Self-suspension failed.");
	return false;
	}

if (theUser->networkClientList.size() <= 1)
	{
	bot->Notice(theClient, "Self-suspension failed.");
	return false;
	}

if(theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
	bot->Notice(theClient, "Self-suspension failed.");
	return false;
	}

// Suspend them.
theUser->setFlag(sqlUser::F_GLOBAL_SUSPEND);
theUser->commit(theClient);
bot->Notice(theClient, "You have been globally suspended and will have level 0 access in all"
	" channels until you are unsuspended by a CService administrator.");

theUser->writeEvent(sqlUser::EV_SUSPEND, theUser, "Self-Suspension");

bot->logAdminMessage("%s (%s) has globally suspended their own account.",
	theClient->getNickUserHost().c_str(), theUser->getUserName().c_str());

return false;
}

} // namespace gnuworld.
