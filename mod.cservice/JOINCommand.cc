/* 
 * JOINCommand.cc 
 *
 * 10/02/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version. Written, and finished.
 *
 * Makes cmaster join a registered channel.
 *
 * Caveats: None
 *
 * $Id: JOINCommand.cc,v 1.3 2001/02/10 22:06:36 gte Exp $
 */


#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

const char JOINCommand_cc_rcsId[] = "$Id: JOINCommand.cc,v 1.3 2001/02/10 22:06:36 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool JOINCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}
 
	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) {
		return false;
	}

	/* 
	 *  Check the channel is actually registered.
	 */

	Channel* tmpChan = Network->findChannel(st[1]);
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
			st[1].c_str());
		return false;
	} 

	/*
	 *  Check the user has sufficient access on this channel.
	 */

	int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
	if (level < level::join)
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str());
		return false;
	} 
 
	theChan->setInChan(true);
	bot->getUplink()->RegisterChannelEvent( theChan->getName(), bot);
	bot->Join(theChan->getName(), theChan->getChannelMode(), theChan->getChannelTS(), true);

	if (tmpChan)
		{
		if(theChan->getFlag(sqlChannel::F_NOOP)) bot->deopAllOnChan(tmpChan);
		if(theChan->getFlag(sqlChannel::F_STRICTOP)) bot->deopAllUnAuthedOnChan(tmpChan);
		}

	return true;
} 

} // namespace gnuworld.
