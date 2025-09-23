/**
 * STATUSCommand.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: STATUSCommand.cc,v 1.48 2007/12/26 20:38:57 kewlio Exp $
 */
#include	<string>
#include	<sstream>
#include	<iostream>
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"
#include	"cservice_config.h"

namespace gnuworld
{
using std::cout ;
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

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

	bot->doCoderStats(theClient);

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
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::status_chan_info,
			string("Channel %s has %d users (%i operators)")).c_str(),
		tmpChan->getName().c_str(),
		tmpChan->size(),
		bot->countChanOps(tmpChan) ) ;
	// If the person has access >200, or is a 1+ admin (or and Oper).
	if ((level >= level::status2) || (admLevel >= 1) || theClient->isOper())
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
if (admLevel > 0)
{
	/* only "*" access can view certain flags */
	if (theChan->getFlag(sqlChannel::F_NOPURGE)) flagsSet += "NOPURGE ";
	if (theChan->getFlag(sqlChannel::F_SPECIAL)) flagsSet += "SPECIAL ";
	if (theChan->getFlag(sqlChannel::F_NOREG)) flagsSet += "NOREG ";
	if (theChan->getFlag(sqlChannel::F_NEVREG)) flagsSet += "NEVERREG ";
	if (theChan->getFlag(sqlChannel::F_CAUTION)) flagsSet += "CAUTION ";
	if (theChan->getFlag(sqlChannel::F_LOCKED)) flagsSet += "LOCKED ";
	if (theChan->getFlag(sqlChannel::F_MIA)) flagsSet += "MIA ";
	if (theChan->getFlag(sqlChannel::F_NOFORCE)) flagsSet += "NOFORCE ";
}
/* anyone with access can view the rest of the flags (below) */
if (theChan->getFlag(sqlChannel::F_SUSPEND)) flagsSet += "SUSPEND ";
if (theChan->getFlag(sqlChannel::F_TEMP)) flagsSet += "TEMP ";
if (theChan->getFlag(sqlChannel::F_VACATION)) flagsSet += "VACATION ";
if (theChan->getFlag(sqlChannel::F_ALWAYSOP)) flagsSet += "ALWAYSOP ";
if (theChan->getFlag(sqlChannel::F_STRICTOP)) flagsSet += "STRICTOP ";
if (theChan->getFlag(sqlChannel::F_NOOP)) flagsSet += "NOOP ";
if (theChan->getFlag(sqlChannel::F_NOVOICE)) flagsSet += "NOVOICE ";
if (theChan->getFlag(sqlChannel::F_AUTOTOPIC)) flagsSet += "AUTOTOPIC ";
if (theChan->getFlag(sqlChannel::F_AUTOJOIN)) flagsSet += "AUTOJOIN ";

if (((level >= level::set::oplog) || (admLevel)) &&
	(theChan->getFlag(sqlChannel::F_OPLOG)))
		flagsSet += "OPLOG ";

if (((level >= level::set::notake) || (admLevel)) &&
        (theChan->getFlag(sqlChannel::F_NOTAKE)))
{
    flagsSet += "NOTAKE (\002REV:\002";
    if (theChan->getNoTake() == 1) flagsSet += "NONE)";
    if (theChan->getNoTake() == 2) flagsSet += "BAN)";
    if (theChan->getNoTake() == 3) flagsSet += "SUSPEND)";
}

bot->Notice(theClient,
    bot->getResponse(theUser, language::status_flags,
        string("Flags set: %s")).c_str(),flagsSet.c_str());

if (((level >= level::set::floodpro) || (admLevel)) &&
		(theChan->getFlag(sqlChannel::F_FLOODPRO)))
{
	stringstream floodperiods;
	if (theChan->getFloodproLevel())
	{
		floodperiods << " FLOODPRO \002(";
		if (theChan->getFloodproLevel() == sqlChannel::FLOODPRO_KICK)
			floodperiods << "KICK";
		if (theChan->getFloodproLevel() == sqlChannel::FLOODPRO_BAN)
			floodperiods << "BAN";
		if (theChan->getFloodproLevel() == sqlChannel::FLOODPRO_GLINE)
			floodperiods << "GLINE";
		floodperiods << ")\002";
	}
	if (theChan->getFlag(sqlChannel::F_FLOODPROGLINE))
		floodperiods << " FLOODPROGLINE";
	if (theChan->getFloodproLevel() == sqlChannel::FLOODPRO_NONE)
		floodperiods << " FLOODPRO";
	floodperiods
		//<< std::hex << theChan->getFloodPro()
		<< " -- (\002MSG:\002"
		<< theChan->getFloodMsg()
		<< ", \002NOTICE:\002"
		<< theChan->getFloodNotice()
		<< ", \002CTCP:\002"
		<< theChan->getFloodCTCP()
		<< ", \002REP:\002"
		<< theChan->getRepeatCount()
		<< ", \002PERIOD:\002"
		<< theChan->getFloodPeriod()
		<< ")"
		<< ends;

	bot->Notice(theClient, floodperiods.str().c_str());
}

if (theChan->getFlag(sqlChannel::F_FLOATLIM))
{
	stringstream floatLim;
	floatLim
    << " FLOATLIM (\002MGN:\002"
	<< theChan->getLimitOffset()
	<< ", \002PRD:\002"
	<< theChan->getLimitPeriod()
	<< ", \002GRC:\002"
	<< theChan->getLimitGrace()
	<< ", \002MAX:\002"
	<< theChan->getLimitMax()
	<< ") "
	<< ends;

	bot->Notice(theClient, floatLim.str().c_str());
}

if (theChan->getFlag(sqlChannel::F_JOINLIM))
{
	stringstream joinLim;
	joinLim << " JOINLIM (\002MAX:\002"
		<< theChan->getLimitJoinMax()
		<< ", \002SECS:\002"
		<< theChan->getLimitJoinSecs()
		<< ", \002PRD:\002"
		<< theChan->getLimitJoinPeriod()
		<< ", \002MODE:\002 "
		<< theChan->getLimitJoinMode().c_str()
		<< ")"
		<< ends;
	bot->Notice(theClient, joinLim.str().c_str());
}

if (theChan->getLimitJoinActive())
	bot->Notice(theClient,
		bot->getResponse(theUser, language::status_joinlim,
			string("Active JOINMODE modes (+%s) will expire in %i seconds")).c_str(),
			theChan->getLimitJoinModeSet().c_str(),
			theChan->getLimitJoinTimeExpire() - time(nullptr));

/* show userflags (if not 'NONE') */
flagsSet.clear();
if (theChan->getUserFlags() == 1) flagsSet += " USERFLAGS=OP ";
else if (theChan->getUserFlags() == 2) flagsSet += " USERFLAGS=VOICE ";

if (!flagsSet.empty())
	bot->Notice(theClient,flagsSet.c_str());

/*
 *  Get a list of authenticated users on this channel.
 */

stringstream authQuery;
authQuery	<< "SELECT users.user_name,levels.access FROM "
		<< "users,levels WHERE users.id = levels.user_id "
		<< "AND levels.channel_id = "
		<< theChan->getID()
		<< " ORDER BY levels.access DESC"
		<< ends;

if( !bot->SQLDb->Exec( authQuery, true ) )
	{
	LOG( ERROR, "STATUSCommand SQL Error:") ;
	LOGSQL_ERROR( bot->SQLDb ) ;
	return false ;
	}

string authList;
string nextPerson;

bool showNick = false;

for (unsigned int i = 0 ; i < bot->SQLDb->Tuples(); i++)
	{
	/*
	 *  Look up this username in the cache.
	 */
	sqlUser* currentUser = bot->getUserRecord(bot->SQLDb->GetValue(i, 0));
	if(currentUser)
		{
		if( !currentUser->isAuthed() )
			{
			continue ;
			}

		nextPerson += bot->SQLDb->GetValue(i, 0);
		nextPerson += "/\002";

		/*
		 * Only show the online nickname if that person is in the target
		 * channel.
		 *
		 * Loop around all people auth'd as this nick and append their nicks
		 */
		for( sqlUser::networkClientListType::iterator ptr = currentUser->networkClientList.begin() ;
			ptr != currentUser->networkClientList.end() ; ++ptr )
			{
			iClient* tmpClient = (*ptr);

			showNick = false;

			/* If we have admin access, always show the nick's of auth'd users. */
			showNick = admLevel;

			/* Only show the nick of the client if he's actually in the channel */
			if (tmpChan) showNick = (tmpChan->findUser(tmpClient) || admLevel);

			if (showNick)
				{
				nextPerson += "";
				nextPerson += tmpClient->getNickName();
				nextPerson += " ";
				}
			}

		nextPerson += "\002[";
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

/*
if (admLevel > level::admin::admin)
{
	 bot->Notice(theClient, "DB TS: %i -- IRC TS: %i",
	 		theChan->getChannelTS(), tmpChan ? tmpChan->getCreationTime() : 0 );
}
*/

return true ;
}

} // namespace gnuworld.
