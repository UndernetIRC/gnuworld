/* STATUSCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"
 
const char STATUSCommand_cc_rcsId[] = "$Id: STATUSCommand.cc,v 1.10 2001/01/31 19:53:25 gte Exp $" ;

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

		float banTotal = bot->banCacheHits + bot->banHits;
		float banEf = (bot->banCacheHits ? ((float)bot->banCacheHits / banTotal * 100) : 0);

		bot->Notice(theClient, "CMaster Channel Services internal status:"); 

		bot->Notice(theClient, "[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%", 
			bot->sqlChannelCache.size(), bot->channelHits, bot->channelCacheHits, chanEf);

		bot->Notice(theClient, "[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%", 
			bot->sqlUserCache.size(), bot->userHits, bot->userCacheHits, userEf);

		bot->Notice(theClient, "[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%", 
			bot->sqlLevelCache.size(), bot->levelHits, bot->levelCacheHits, levelEf);

		bot->Notice(theClient, "[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%", 
			bot->sqlBanCache.size(), bot->banHits, bot->banCacheHits, banEf);
			
		bot->Notice(theClient, "Last recieved Channel NOTIFY: %i", bot->lastChannelRefresh);
		bot->Notice(theClient, "Last recieved User NOTIFY: %i", bot->lastUserRefresh);
		bot->Notice(theClient, "Last recieved Level NOTIFY: %i", bot->lastLevelRefresh);
		bot->Notice(theClient, "Last recieved Ban NOTIFY: %i", bot->lastBanRefresh);

		bot->Notice(theClient, "Custom data containers allocated: %i", bot->customDataAlloc);

		bot->Notice(theClient, "\002Uptime:\002 %s",  bot->prettyDuration(bot->getUplink()->getStartTime() + bot->dbTimeOffset).c_str());
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

	int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
	int admLevel = bot->getAdminAccessLevel(theUser); // Let authenticated admins view status also.
	if ((level < level::status) && (admLevel <= 0))
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
		if ((level >= 400) || (admLevel >= 600)) // If the person has access >400, or is a 600+ admin.
		{
			bot->Notice(theClient, "Channel %s has %d users (TBA operators)",
				tmpChan->getName().c_str(), tmpChan->size() ) ;
	 
			bot->Notice(theClient, "Mode is: %s",
				tmpChan->getModeString().c_str() ) ;
		}
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
