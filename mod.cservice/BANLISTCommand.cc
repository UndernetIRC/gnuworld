/*
 * BANLISTCommand.cc
 *
 * 30/12/2000 - David Henriksen <david@itwebnet.dk>
 * Initial Version.
 *
 * Lists the banlist of a channel, not the internal one, but the
 * active channel banlist.
 *
 * Caveats: None.
 *
 * $Id: BANLISTCommand.cc,v 1.15 2001/07/30 18:40:00 gte Exp $
 */

#include        <string>

#include        "StringTokenizer.h"
#include        "cservice.h"
#include        "Network.h"
#include        "levels.h"
#include        "responses.h"

const char BANLISTCommand_cc_rcsId[] = "$Id: BANLISTCommand.cc,v 1.15 2001/07/30 18:40:00 gte Exp $" ;

namespace gnuworld
{

using std::string ;
using namespace level;

bool BANLISTCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, false);

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_not_reg).c_str(),
		st[1].c_str());
	return false;
	}

Channel* tmpChan = Network->findChannel(st[1]);
if (!tmpChan)
	{
	bot->Notice(theClient, bot->getResponse(theUser, language::chan_is_empty).c_str(),
		st[1].c_str());
	return false;
	}

/*
 * Check we have enough access to view the banlist.
 */

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);

if (!level) return false;

for(Channel::const_banIterator ptr = tmpChan->banList_begin();
	ptr != tmpChan->banList_end(); ++ptr)
	{
	bot->Notice(theClient, "%s", (*ptr).c_str());
	}

if( 0 == tmpChan->banList_size() )
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::ban_list_empty,
			string("%s: ban list is empty.")).c_str(),
		st[1].c_str());
	}
else	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::end_ban_list,
			string("%s: End of ban list")).c_str(),
		st[1].c_str());
	}

return true ;
}

} // namespace gnuworld.
