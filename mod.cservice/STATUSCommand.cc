/* STATUSCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"
 
const char STATUSCommand_cc_rcsId[] = "$Id: STATUSCommand.cc,v 1.2 2001/01/03 20:36:03 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool STATUSCommand::Exec( iClient* theClient, const string& Message )
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

	if (st[1] == "*") 
	{
		/*
		 *  Special case, display admin stats.
		 */

		if (bot->getAdminAccessLevel(theUser) <= level::admin::helper) // Don't show if they don't have any admin access.
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
				st[1].c_str());
			return false; 
		}

		/*
		 *  Show some fancy stats.
		 */

		float chanTotal = bot->channelCacheHits + bot->channelHits;
		float chanEf = (bot->channelCacheHits ? ((float)bot->channelCacheHits / chanTotal * 100) : 0);

		float userTotal = bot->userCacheHits + bot->userHits;
		float userEf = (bot->userCacheHits ? ((float)bot->userCacheHits / userTotal * 100) : 0);

		float levelTotal = bot->levelCacheHits + bot->levelHits;
		float levelEf = (bot->levelCacheHits ? ((float)bot->levelCacheHits / levelTotal * 100) : 0);

		bot->Notice(theClient, "CMaster Channel Services internal status:");
		bot->Notice(theClient, "Channel record requests: %i", bot->channelHits);
		bot->Notice(theClient, "Channel record cache hits: %i (%.2f%% efficiency)", bot->channelCacheHits, chanEf);

		bot->Notice(theClient, "User record requests: %i", bot->userHits);
		bot->Notice(theClient, "User record cache hits: %i (%.2f%% efficiency)", bot->userCacheHits, userEf);

		bot->Notice(theClient, "Access Level record requests: %i", bot->levelHits);
		bot->Notice(theClient, "Access Level record cache hits: %i (%.2f%% efficiency)", bot->levelCacheHits, levelEf);
		return true;
	}

	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
			st[1].c_str());
		return false;
	} 

	/*
	 *  Check the user has sufficient access on this channel.
	 */

	int level = bot->getAccessLevel(theUser, theChan);
	if (level < level::status)
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str());
		return false;
	} 

 	/*
	 *  Display some fancy info about the channel. 
	 */

	Channel* tmpChan = Network->findChannel(theChan->getName()); 

	if (tmpChan) 
	{
		bot->Notice(theClient, "Channel %s has %d users (TBA operators)",
			tmpChan->getName().c_str(), tmpChan->size() ) ;

		bot->Notice(theClient, "Mode is: %s",
			tmpChan->getModeString().c_str() ) ;
	}

	bot->Notice(theClient, "MassDeopPro: %i, FloodPro: %i", 
		theChan->getMassDeopPro(), theChan->getFloodPro());
 
	string flagsSet;
	flagsSet = "";
	if (theChan->getFlag(sqlChannel::F_NOPURGE)) flagsSet += "NOPURGE ";
	if (theChan->getFlag(sqlChannel::F_SPECIAL)) flagsSet += "SPECIAL ";
	if (theChan->getFlag(sqlChannel::F_NOREG)) flagsSet += "NOREG ";
	if (theChan->getFlag(sqlChannel::F_NEVREG)) flagsSet += "NEVERREG ";
	if (theChan->getFlag(sqlChannel::F_SUSPEND)) flagsSet += "SUSPEND ";
	if (theChan->getFlag(sqlChannel::F_TEMP)) flagsSet += "TEMP ";
	if (theChan->getFlag(sqlChannel::F_CAUTION)) flagsSet += "CAUTION ";
	if (theChan->getFlag(sqlChannel::F_VACATION)) flagsSet += "VACATION ";
	if (theChan->getFlag(sqlChannel::F_ALWAYSOP)) flagsSet += "ALWAYSOP ";
	if (theChan->getFlag(sqlChannel::F_STRICTOP)) flagsSet += "STRICTOP ";
	if (theChan->getFlag(sqlChannel::F_NOOP)) flagsSet += "NOOP ";
	if (theChan->getFlag(sqlChannel::F_AUTOTOPIC)) flagsSet += "AUTOTOPIC ";
	if (theChan->getFlag(sqlChannel::F_OPONLY)) flagsSet += "OPONLY ";
	if (theChan->getFlag(sqlChannel::F_AUTOJOIN)) flagsSet += "AUTOJOIN ";

	bot->Notice(theClient, "Flags set: %s",flagsSet.c_str()); 
	bot->Notice(theClient, "Auth: TBA");
	return true ;
} 

} // namespace gnuworld.
