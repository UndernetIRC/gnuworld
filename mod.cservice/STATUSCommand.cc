/* STATUSCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"
 
const char STATUSCommand_cc_rcsId[] = "$Id: STATUSCommand.cc,v 1.23 2001/03/06 23:44:00 gte Exp $" ;

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

		if (!bot->getAdminAccessLevel(theUser)) // Don't show if they don't have any admin access.
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

		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_tagline,
				string("CMaster Channel Services internal status:"))); 

		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_chan_rec,
				string("[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%")).c_str(), 
			bot->sqlChannelCache.size(), bot->channelHits, bot->channelCacheHits, chanEf);

		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_user_rec,
				string("[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%")).c_str(), 
			bot->sqlUserCache.size(), bot->userHits, bot->userCacheHits, userEf);

		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_access_rec,
				string("[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%")).c_str(), 
			bot->sqlLevelCache.size(), bot->levelHits, bot->levelCacheHits, levelEf);

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::status_ban_rec,
				string("[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%")).c_str(), 
			bot->sqlBanCache.size(), bot->banHits, bot->banCacheHits, banEf);
			
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_last_chan_not,
				string("Last recieved Channel NOTIFY: %i")).c_str(), bot->lastChannelRefresh);
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_last_user_not,
				string("Last recieved User NOTIFY: %i")).c_str(), bot->lastUserRefresh);
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_last_lvl_not,
				string("Last recieved Level NOTIFY: %i")).c_str(), bot->lastLevelRefresh);
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_last_ban_not,
				string("Last recieved Ban NOTIFY: %i")).c_str(), bot->lastBanRefresh);

		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_data_alloc,
				string("Custom data containers allocated: %i")).c_str(), bot->customDataAlloc);

		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_uptime,
				string("\002Uptime:\002 %s")).c_str(),  
			bot->prettyDuration(bot->getUplink()->getStartTime() + bot->dbTimeOffset).c_str());


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
	if ((level < level::status) && (admLevel <= 0) && !theClient->isOper())
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
		// If the person has access >200, or is a 1+ admin (or and Oper).
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::status_chan_info,
				string("Channel %s has %d users (%i operators)")).c_str(),
			tmpChan->getName().c_str(), tmpChan->size(), bot->countChanOps(tmpChan) ) ;
		if ((level >= 200) || (admLevel >= 1) || theClient->isOper()) 
		{
	 
			bot->Notice(theClient, 
				bot->getResponse(theUser,
					language::status_mode,
					string("Mode is: %s")).c_str(),
				tmpChan->getModeString().c_str() ) ;
		}

		if (admLevel >= 1)
		{
			/*
			 *  Execute a quick query to find the last 3 join/part events that occured
			 *  on this channel.
			 */

			strstream theQuery;
			theQuery << "SELECT * FROM channellog WHERE (event = "
			<< sqlChannel::EV_JOIN << " OR event = "
			<< sqlChannel::EV_PART << " OR event = "
			<< sqlChannel::EV_OPERJOIN << " OR event = "
			<< sqlChannel::EV_OPERPART << ")"
			<< " AND channelID = " 
			<< theChan->getID() 
			<< " ORDER BY ts DESC LIMIT 3"
			<< ends;

			elog << "sqlQuery> " << theQuery.str() << endl;

			ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ; 
			delete[] theQuery.str();

			if( PGRES_TUPLES_OK == status ) 
				{
				if (bot->SQLDb->Tuples() > 0) bot->Notice(theClient, "Last 3 channel events:");
				for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
					{
						string type = "";
						switch( atoi(bot->SQLDb->GetValue(i, 2)) )
						{
							case sqlChannel::EV_MISC:
							{
								type = "MISC";
								break;
							}
							case sqlChannel::EV_JOIN:
							{
								type = "JOIN";
								break;
							}
							case sqlChannel::EV_OPERJOIN:
							{
								type = "OPERJOIN";
								break;
							}
							case sqlChannel::EV_OPERPART:
							{
								type = "OPERPART";
								break;
							} 
							case sqlChannel::EV_PART:
							{
								type = "PART";
								break;
							}
							case sqlChannel::EV_FORCE:
							{
								type = "FORCE";
								break; 
							}
						}
						bot->Notice(theClient, "%s ago: %s %s",
							bot->prettyDuration( atoi(bot->SQLDb->GetValue(i, 0)) ).c_str(), 
							type.c_str(),
							bot->SQLDb->GetValue(i, 3));
					}
				} 

		} // Admin access.
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

	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::status_flags,
			string("Flags set: %s")).c_str(),flagsSet.c_str()); 

	/*
	 *  Get a list of authenticated users on this channel.
	 */

	strstream authQuery;
	strstream authList;
	authQuery << "SELECT users.user_name,levels.access FROM users,levels WHERE users.id = levels.user_id "
			 << "AND levels.channel_id = " << theChan->getID() << ends; 

	elog << "sqlQuery> " << authQuery.str() << endl;

	ExecStatusType status = bot->SQLDb->Exec( authQuery.str() ) ; 
	delete[] authQuery.str();

	/* TODO: Expand to multiline support. */

	if( PGRES_TUPLES_OK == status ) 
	{ 
			for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
				{
					/*
					 *  Look up this username in the cache.
					 */

					cservice::sqlUserHashType::iterator ptr = bot->sqlUserCache.find(bot->SQLDb->GetValue(i, 0));
					if(ptr != bot->sqlUserCache.end())
					{ 
						iClient* tmpClient = ptr->second->isAuthed();
						if(tmpClient)
						{
							authList << bot->SQLDb->GetValue(i, 0) 
							<< "/"
							<< tmpClient->getNickName()
							<< " (" << bot->SQLDb->GetValue(i, 1) << ") "; 
						}
					}
				}
	} 

	authList << ends;
	bot->Notice(theClient, "Auth: %s", authList.str());

	return true ;
} 

} // namespace gnuworld.
