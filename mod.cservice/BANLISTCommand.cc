/* 
 * BANLISTCommand.cc 
 *
 * 30/12/2000 - David Henriksen <david@itwebnet.dk>
 * Initial Version.
 *
 * Lists the banlist of a channel, not the internal one, but the
 * active channel banlist. This will only work if theClient is in the
 * specified channel.
 * Note that the specified channel doesn't have to be registered.
 *
 * Caveats: None.
 *
 * $Id: BANLISTCommand.cc,v 1.3 2001/01/27 20:16:40 dan_karrels Exp $
 */

#include        <string>

#include        "StringTokenizer.h"
#include        "ELog.h" 
#include        "cservice.h" 
#include        "Network.h"
#include        "levels.h"
#include        "responses.h"

const char BANLISTCommand_cc_rcsId[] = "$Id: BANLISTCommand.cc,v 1.3 2001/01/27 20:16:40 dan_karrels Exp $" ;

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
	
/*
 *  Fetch the sqlUser record attached to this client. If there isn't one,
 *  they aren't logged in - tell them they should be.
 */
	
sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false;
	}
 
Channel* tmpChan = Network->findChannel(st[1]); 
if (!tmpChan) 
	{
	bot->Notice(theClient, bot->getResponse(theUser, language::chan_is_empty).c_str(), 
		st[1].c_str());
	return false;
	}
	
/* Check if the user is in the channel. */
vector< string > channels;
bool inChan = 0;

for(iClient::const_channelIterator ptr = theClient->channels_begin();
	ptr != theClient->channels_end(); ++ptr)
	{
	if(strcasecmp(st[1].c_str(), (*ptr)->getName().c_str()) == 0)
		{
		inChan = true;
		}
	channels.push_back((*ptr)->getName());
	} // for()

/* Do only show the banlist, if the user is in the channel. */
if( !inChan )
	{
	bot->Notice(theClient, "%s: You are not in that channel.",
		st[1].c_str());
	return true;
	}

for(Channel::const_banIterator ptr = tmpChan->banList_begin();
	ptr != tmpChan->banList_end(); ++ptr)
	{
	bot->Notice(theClient, "%s", (*ptr).c_str());
	}

if( 0 == tmpChan->banList_size() )
	{
	bot->Notice(theClient, "%s: ban list is empty.",
		st[1].c_str());
	}
else	{
	bot->Notice(theClient, "%s: End of ban list",
		st[1].c_str());
	}

return true ;
} 
	
} // namespace gnuworld.

