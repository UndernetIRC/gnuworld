/* STATUSCommand.cc */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"
#include	"cservice_config.h"

const char STATUSCommand_cc_rcsId[] = "$Id: STATUSCommand.cc,v 1.37 2002/01/05 01:00:49 gte Exp $" ;

namespace gnuworld
{
using std::string ;

bool STATUSCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.STATUS");

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

/*
 *  Check the channel is actually registered.
 */

if (st[1] == "*")
	{
	/*
	 *  Special case, display admin stats.
	 */

	/* Don't show if they don't have any admin or coder-com access. */
	if (!bot->getAdminAccessLevel(theUser) && !bot->getCoderAccessLevel(theUser))
		{
		bot->Notice(theClient,
			bot->getResponse(theUser, language::chan_not_reg).c_str(),
			st[1].c_str());
		return false;
		}

	/*
	 *  Show some fancy stats.
	 */

	float chanTotal = bot->channelCacheHits + bot->channelHits;
	float chanEf = (bot->channelCacheHits ?
		((float)bot->channelCacheHits / chanTotal * 100) : 0);

	float userTotal = bot->userCacheHits + bot->userHits;
	float userEf = (bot->userCacheHits ?
		((float)bot->userCacheHits / userTotal * 100) : 0);

	float levelTotal = bot->levelCacheHits + bot->levelHits;
	float levelEf = (bot->levelCacheHits ?
		((float)bot->levelCacheHits / levelTotal * 100) : 0);

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::status_tagline,
			string("CMaster Channel Services internal status:")));

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::status_chan_rec,
			string("[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%")).c_str(),
		bot->sqlChannelCache.size(),
		bot->channelHits,
		bot->channelCacheHits,
		chanEf);

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::status_user_rec,
			string("[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%")).c_str(),
		bot->sqlUserCache.size(),
		bot->userHits,
		bot->userCacheHits,
		userEf);

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::status_access_rec,
			string("[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%")).c_str(),
		bot->sqlLevelCache.size(),
		bot->levelHits,
		bot->levelCacheHits,
		levelEf);


	bot->Notice(theClient,"Last recieved Channel NOTIFY: %s",
			bot->prettyDuration(bot->lastChannelRefresh).c_str());

	bot->Notice(theClient, "Last recieved User NOTIFY: %s",
			bot->prettyDuration(bot->lastUserRefresh).c_str());

	bot->Notice(theClient, "Last recieved Level NOTIFY: %s",
			bot->prettyDuration(bot->lastLevelRefresh).c_str());

	bot->Notice(theClient, "Last recieved Ban NOTIFY: %s",
			bot->prettyDuration(bot->lastBanRefresh).c_str());

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::status_data_alloc,
			string("Custom data containers allocated: %i")).c_str(),
			bot->customDataAlloc);

	float joinTotal = ((float)bot->joinCount / (float)Network->channelList_size()) * 100;
	bot->Notice(theClient, "I am in %i channels out of %i on the network. (%.2f%%)",
		bot->joinCount, Network->channelList_size(), joinTotal);

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::status_uptime,
			string("\002Uptime:\002 %s")).c_str(),
		bot->prettyDuration(bot->getUplink()->getStartTime()
			+ bot->dbTimeOffset).c_str());

	return true;
	}

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_not_reg).c_str(),
		st[1].c_str());
	return false;
	}

/*
 *  Check the user has sufficient access on this channel.
 */

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);

// Let authenticated admins view status also.
int admLevel = bot->getAdminAccessLevel(theUser);

if ((level < level::status) && (admLevel <= 0) && !theClient->isOper())
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::insuf_access).c_str());
	return false;
	}

/*
 *  Display some fancy info about the channel.
 */
Channel* tmpChan = Network->findChannel(theChan->getName());

if (tmpChan)
	{
	// If the person has access >200, or is a 1+ admin (or and Oper).
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::status_chan_info,
			string("Channel %s has %d users (%i operators)")).c_str(),
		tmpChan->getName().c_str(),
		tmpChan->size(),
		bot->countChanOps(tmpChan) ) ;
	if ((level >= 200) || (admLevel >= 1) || theClient->isOper())
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::status_mode,
				string("Mode is: %s")).c_str(),
			tmpChan->getModeString().c_str() ) ;
		}

	/*
	 * Are we on this channel?
	 */

	if (!theChan->getInChan())
		{
			bot->Notice(theClient, "I'm \002not\002 in this channel.");
		} else
		{
			bot->Notice(theClient, "I'm currently in this channel.");
		}

	} // if( tmpChan )
	else
	{
		bot->Notice(theClient, "I'm \002not\002 in this channel.");
	}


bot->Notice(theClient, "MassDeopPro: %i", theChan->getMassDeopPro());

string flagsSet;
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
if (theChan->getFlag(sqlChannel::F_AUTOJOIN)) flagsSet += "AUTOJOIN ";
if (theChan->getFlag(sqlChannel::F_LOCKED)) flagsSet += "LOCKED ";
if (theChan->getFlag(sqlChannel::F_FLOATLIM))
	{
	strstream floatLim;
	floatLim
	<< "FLOATLIM (MGN:"
	<< theChan->getLimitOffset()
	<< ", PRD:"
	<< theChan->getLimitPeriod()
	<< ", GRC:"
	<< theChan->getLimitGrace()
	<< ", MAX:"
	<< theChan->getLimitMax()
	<< ")"
	<< ends;
	flagsSet += floatLim.str();
	}

bot->Notice(theClient,
	bot->getResponse(theUser, language::status_flags,
		string("Flags set: %s")).c_str(),flagsSet.c_str());

/*
 *  Get a list of authenticated users on this channel.
 */

strstream authQuery;
authQuery	<< "SELECT users.user_name,levels.access FROM "
		<< "users,levels WHERE users.id = levels.user_id "
		<< "AND levels.channel_id = "
		<< theChan->getID()
		<< " ORDER BY levels.access DESC"
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlQuery> "
		<< authQuery.str()
		<< endl;
#endif

ExecStatusType status = bot->SQLDb->Exec( authQuery.str() ) ;
delete[] authQuery.str();

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "STATUS> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

string authList;
string nextPerson;

bool showNick = false;

for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
	{
	/*
	 *  Look up this username in the cache.
	 */

	cservice::sqlUserHashType::iterator ptr =
		bot->sqlUserCache.find(bot->SQLDb->GetValue(i, 0));

	if(ptr != bot->sqlUserCache.end())
		{
		iClient* tmpClient = ptr->second->isAuthed();
		if( !tmpClient )
			{
			continue ;
			}

		nextPerson += bot->SQLDb->GetValue(i, 0);

		/*
		 * Only show the online nickname if that person is in the target
		 * channel.
		 */

		showNick = false;
		if (tmpChan) showNick = (tmpChan->findUser(tmpClient) || admLevel);

		if (showNick)
			{
			nextPerson += "/\002";
			nextPerson += tmpClient->getNickName();
			nextPerson += "\002";
			}

		nextPerson += " [";
		nextPerson += bot->SQLDb->GetValue(i, 1);
		nextPerson += "] ";

		/*
		 *  Will this string overflow our Notice buffer?
		 *  If so, dump it now..
		 */
		if(nextPerson.size() + authList.size() > 400)
			{
			bot->Notice(theClient, "Auth: %s", authList.c_str());
			authList.erase( authList.begin(), authList.end() );
			}

		/*
		 * Add it on to our list.
		 */

		authList += nextPerson;
		nextPerson.erase( nextPerson.begin(), nextPerson.end() );
		}


	} // for()

bot->Notice(theClient, "Auth: %s", authList.c_str());

/*
 *  Finally(!) display a quick list of everyone 'forced' on the
 *  channel.
 */

if (admLevel >= 1)
{
	for(sqlChannel::forceMapType::const_iterator ptr = theChan->forceMap.begin();
		ptr != theChan->forceMap.end(); ++ptr)
		{
			bot->Notice(theClient, "Force: %s (%i)",
				ptr->second.second.c_str(), ptr->second.first);
		}
}

return true ;
}

} // namespace gnuworld.
