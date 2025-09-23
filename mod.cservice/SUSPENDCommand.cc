/**
 * SUSPENDCommand.cc
 *
 * 06/01/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version, done with a fever :/
 *
 * Suspends an user on the specified channel, if suspend duration 0
 * is defined, the user will be unsuspended.
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
 * $Id: SUSPENDCommand.cc,v 1.29 2010/04/10 18:56:06 danielaustin Exp $
 */
#include	<iostream>
#include	<string>
#include	<ctime>
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

namespace gnuworld
{
using std::string ;
using namespace level;

bool SUSPENDCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SUSPEND");

StringTokenizer st( Message ) ;
if( st.size() < 3 )
{
	Usage(theClient);
	return true;
}

/* Is the user authenticated? */
sqlUser* theUser = bot->isAuthed(theClient, true);
if(!theUser)
{
	return false;
}

/*
 * Trying to suspend a user, or a channel?
 * If there is no #, check this person's admin access..
 * If it passes, we can suspend a whole user account. <g>
 */

if ((st[1][0] != '#') && (st[1][0] != '*'))
{
	/* Got enough admin access? */
	int level = bot->getAdminAccessLevel(theUser);
	if (level == 0)
	{
		Usage(theClient);
		return true;
	}
    if ((level > 0) && (level < level::globalsuspend))
    {
    	bot->Notice(theClient,
    			bot->getResponse(theUser, language::insuf_access,
    					string("Sorry, you have insufficient access to perform that command.")));
    	return true;
    }

	/* Does this user account even exist? */
	sqlUser* targetUser = bot->getUserRecord(st[1]);
	if (!targetUser)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser, language::not_registered,
				string("I don't know who %s is")).c_str(),
		    	st[1].c_str());
		return true;
	}

	/*
	 *  Check the target's admin access, if its >= ours, don't
	 *  allow it. :)
	 */

	int targetLevel = bot->getAdminAccessLevel(targetUser);
	if (targetLevel >= level)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::suspend_access_higher,
				string("Cannot suspend a user with equal or higher access than your own.")));
		return false;
	}

	if (targetUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
		bot->Notice(theClient, "%s is already suspended, you could always try execution?",
			targetUser->getUserName().c_str());
		return true;
	}

	// Suspend them.
	targetUser->setFlag(sqlUser::F_GLOBAL_SUSPEND);
	bot->sendAccountFlags(targetUser);
	targetUser->commit(theClient);
	bot->Notice(theClient, "%s has been globally suspended and will have level 0 access in all"
		" channels until unsuspended.",
		targetUser->getUserName().c_str());

	targetUser->writeEvent(sqlUser::EV_SUSPEND, theUser, st.assemble(2));

	//Deop the all newly suspended clients
	for (sqlUser::networkClientListType::iterator cliPtr = targetUser->networkClientList.begin();
			cliPtr != targetUser->networkClientList.end() ; ++cliPtr )
	{
		for (iClient::channelIterator chItr = (*cliPtr)->channels_begin(); chItr != (*cliPtr)->channels_end(); ++chItr)
		{
			ChannelUser* theChannelUser = (*chItr)->findUser(*cliPtr);
			sqlChannel* theChan = bot->getChannelRecord((*chItr)->getName());
			if ((theChan) && (theChan->getFlag(sqlChannel::F_STRICTOP)) && (theChannelUser->getMode(ChannelUser::MODE_O)))
			{
				sqlLevel* tmpLevel = bot->getLevelRecord(targetUser, theChan);
				if (tmpLevel)
				{
					bot->removeLevelCache(tmpLevel);
					delete(tmpLevel);
				}
				bot->deopSuspendedOnChan(*chItr,targetUser);
			}
		}
		bot->Notice((*cliPtr),"Your user account has been globally suspended.");
	}

	bot->logAdminMessage("%s (%s) has globally suspended %s's user account.",
		theClient->getNickName().c_str(), theUser->getUserName().c_str(),
		targetUser->getUserName().c_str());

	return true;
} //end of Global user suspend, begin channel suspend

if( st.size() < 4 )
{
	Usage(theClient);
	return true;
}

/* Is the channel registered? */
sqlChannel* theChan = bot->getChannelRecord(st[1]);
if(!theChan)
{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_not_reg,
			string("Sorry, %s isn't registered with me.")).c_str(),
		st[1].c_str());
	return false;
}

/* Check level. */
int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
int suspendLevel = level;
if ((level < level::suspend) || (( st[1] == "*" ) && (level < adminlevel::suspend)))
{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
	return false;
}

/* Check whether the user is in the access list. */
sqlUser* Target = bot->getUserRecord(st[2]);
if(!Target)
{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::not_registered,
			string("I don't know who %s is")).c_str(),
	    	st[2].c_str());
	return true;
}

int usrLevel = bot->getAccessLevel(Target, theChan);
if (!usrLevel)
{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::doesnt_have_access,
			string("%s doesn't appear to have access in %s.")).c_str(),
	    	Target->getUserName().c_str(),
		theChan->getName().c_str());
	return true;
}

if (level <= usrLevel)
{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::suspend_access_higher,
			string("Cannot suspend a user with equal or higher access than your own.")));
	return false;
}

time_t finalDuration;
string suspReason;

if (!IsTimeSpec(st[3])) //this means only a reason was specified, no duration, no level and we use the defaults
{
	suspReason = st.assemble(3);
	finalDuration = 3 * 3600;
}
else
	finalDuration = extractTime(st[3], 60);

/* If we have a length at least of 5, then if the 5th param is a number then it is the susp level
 * else the reason is specified
 */
if ((st.size() > 4) && (suspReason.empty()))
{
	if (IsNumeric(st[4])) suspendLevel = atoi(st[4].c_str());
	else suspReason = st.assemble(4);
}

if ((st.size() > 5) && (suspReason.empty()))
{
	suspReason = st.assemble(5);
}
/* Greater than a year? */
if(finalDuration > 32140800 || finalDuration < 0)
{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::inval_suspend_dur,
			string("Invalid suspend duration.")));
	return true;
}

sqlLevel* aLevel = bot->getLevelRecord(Target, theChan);
if (suspReason.empty()) suspReason = "No reason supplied.";

if( 0 == finalDuration )
{
	/*
	 * Was this suspension set with a higher suspend level?
	 */
	if (aLevel->getSuspendLevel() > level)
	{
		bot->Notice(theClient,
			"Cannot unsuspend a user that was suspended at a higher level than your own access.");
		return false;
	}
	if ((suspReason.size() < 2) || (suspReason.size() > 300))
	{
		//bot->Notice(theClient,"Your reason must be 2 - 300 charcters long.");
		bot->Notice(theClient, bot->getResponse(theUser,language::reason_must).c_str(),2,300);
		return false;
	}
	aLevel->setSuspendExpire(finalDuration);
	aLevel->setSuspendBy(string());
	aLevel->setSuspendReason(string());
	aLevel->setLastModif(bot->currentTime());
	aLevel->setLastModifBy( string( "("
		+ theUser->getUserName()
		+ ") "
		+ theClient->getNickUserHost() ) );

	if( !aLevel->commit() )
	{
		bot->Notice( theClient,
			"Error updating channel status." );
		elog	<< "UNSUSPEND> SQL error"
			<< std::endl;
		return false;
	}

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::susp_cancelled,
			string("SUSPENSION for %s is cancelled")).c_str(),
		Target->getUserName().c_str());
	if (Target != theUser)
	{
		bot->NoteAllAuthedClients(Target, bot->getResponse(Target,language::acc_unsusp).c_str(), theChan->getName().c_str());
		// Announce the manager about the new access change
		if (level < 500)
		{
			string theMessage = TokenStringsParams("%s unsuspended %s's access on channel %s",
					theUser->getUserName().c_str(), Target->getUserName().c_str(), theChan->getName().c_str());
			bot->NoteChannelManager(theChan, theMessage.c_str());
		}
	}

	return true;
}

if (aLevel->getSuspendExpire() > bot->currentTime())
{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::already_susp,
			string("%s is already suspended on %s")).c_str(),
		Target->getUserName().c_str(),
		theChan->getName().c_str());
	return false;
}

if ((suspReason.size() < 2) || (suspReason.size() > 300))
{
	//bot->Notice(theClient,"Your reason must be 2 - 300 charcters long.");
	bot->Notice(theClient, bot->getResponse(theUser,language::reason_must).c_str(),2,300);
	return false;
}

/*
 * Check any access level supplied, default to our access
 * if not.
 */

if ((suspendLevel > level) || (suspendLevel > 500) || (suspendLevel <= 0))
	suspendLevel = level;

aLevel->setSuspendExpire(finalDuration + bot->currentTime());
aLevel->setSuspendBy(theClient->getNickUserHost());
aLevel->setSuspendLevel(suspendLevel);
aLevel->setSuspendReason(suspReason);
aLevel->setLastModif(bot->currentTime());
aLevel->setLastModifBy( string( "("
	+ theUser->getUserName()
	+ ") "
	+ theClient->getNickUserHost() ) );

aLevel->commit();

Channel* tmpChan = Network->findChannel(theChan->getName());
if ((tmpChan) && (theChan->getFlag(sqlChannel::F_STRICTOP)))
	bot->deopSuspendedOnChan(tmpChan,Target);

bot->Notice(theClient,
	bot->getResponse(theUser,
		language::susp_set,
		string("SUSPENSION for %s will expire in %s")).c_str(),
	Target->getUserName().c_str(),
	prettyDuration(bot->currentTime() - finalDuration ).c_str());
	if (Target != theUser)
	{
		bot->NoteAllAuthedClients(Target, bot->getResponse(Target,language::acc_susp).c_str(), theChan->getName().c_str());
		// Announce the manager about the new access change
		if (level < 500)
		{
			string theMessage = TokenStringsParams("%s suspended %s's access on channel %s",
					theUser->getUserName().c_str(), Target->getUserName().c_str(), theChan->getName().c_str());
			bot->NoteChannelManager(theChan, theMessage.c_str());
		}
	}

return true ;
}

} // namespace gnuworld.
