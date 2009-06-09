/**
 * CHANINFOCommand.cc
 *
 * 29/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Template.
 *
 * 30/12/2000 - David Henriksen <david@itwebnet.dk>
 * Started and finished the command. Showing all owners by a
 * SQL Query which returns all the level 500s of the channel.
 *
 * Caveats: Need to determine if the query is aimed at a #
 * or a user. :)
 *
 * Command is aliased "INFO".
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
 * $Id: CHANINFOCommand.cc,v 1.62 2009/06/09 15:40:29 mrbean_ Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"dbHandle.h"
#include	"cservice_config.h"

const char CHANINFOCommand_cc_rcsId[] = "$Id: CHANINFOCommand.cc,v 1.62 2009/06/09 15:40:29 mrbean_ Exp $" ;

namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

static const char* queryHeader = "SELECT channels.name,users.user_name,levels.access,users_lastseen.last_seen FROM levels,channels,users,users_lastseen ";
static const char* queryString = "WHERE levels.channel_id=channels.id AND users.id=users_lastseen.user_id AND levels.access = 500 AND levels.user_id = users.id ";

bool CHANINFOCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.INFO");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* tmpUser = bot->isAuthed(theClient, true);
if (!tmpUser)
{
	return false;
}

int adminAccess = 0;
if (tmpUser) adminAccess = bot->getAdminAccessLevel(tmpUser);

/*
 *  Are we checking info about a user or a channel?
 */

// Did we find a '#' ?
if( string::npos == st[ 1 ].find_first_of( '#' ) )
	{
	// Nope, look by user then.
	sqlUser* theUser = bot->getUserRecord(st[1]);

	if (!theUser)
		{
		bot->Notice(theClient,
			bot->getResponse(tmpUser,
				language::not_registered,
				string("The user %s doesn't appear to be registered.")).c_str(),
			st[1].c_str());
		return true;
		}

	/* fetch admin access level (manually to avoid various checks) for use for IP hiding ONLY */
	unsigned short tmpadminLevel;
	sqlChannel* adminChan = bot->getChannelRecord("*");
	if (!adminChan)
	{
		/* cant find admin channel for some reason, assume no access of course */
		tmpadminLevel = 0;
	} else {
		/* found admin channel, try to get the level record */
		sqlLevel* adminLev = bot->getLevelRecord(theUser, adminChan);
		if (!adminLev)
		{
			/* no level record, assume no access */
			tmpadminLevel = 0;
		} else {
			/* found it, set it */
			tmpadminLevel = adminLev->getAccess();
		}
	}

	/* get the language STRING */
	stringstream s;
	int langId = theUser->getLanguageId();
	s	<< langId
		<< ends ;
	string langString = s.str();

	for (cservice::languageTableType::iterator ptr = bot->languageTable.begin();
		ptr != bot->languageTable.end(); ptr++)
	{
		if (ptr->second.first == langId)
		{
			langString = ptr->first;
			break;
		}
	}

	/* build up a flag string */
	string flagsSet;

	if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND)) 
		flagsSet += "SUSPEND ";
	if (theUser->getFlag(sqlUser::F_INVIS))
		flagsSet += "INVISIBLE ";
	if (theUser->getFlag(sqlUser::F_NOADDUSER))
		flagsSet += "NOADDUSER ";
	/* flags only visible to *1+ users */
	if (adminAccess)
	{
		if (theUser->getFlag(sqlUser::F_FRAUD))
			flagsSet += "FRAUD ";
		if (theUser->getFlag(sqlUser::F_NOPURGE))
			flagsSet += "NOPURGE ";
		if (theUser->getFlag(sqlUser::F_ALUMNI))
			flagsSet += "ALUMNI ";
		if (theUser->getFlag(sqlUser::F_NOADMIN))
			flagsSet += "DISABLEAUTH ";
	}
	/* flags with variables */
	if (langString.size() > 0)
		flagsSet += "LANG=" + langString + " ";
	/* flags with variables for admins (or self-viewing) only */
	if (adminAccess || (tmpUser == theUser))
	{
		int maxLogins = theUser->getMaxLogins();
		stringstream ss;
		ss	<< maxLogins;
//			<< ends ;
		if (maxLogins > 1)
			flagsSet += "MAXLOGINS=" + ss.str() + " ";
	
		stringstream autoInviteQuery;

		autoInviteQuery	<< "SELECT channel_id from levels"
				<< " where user_id = " << theUser->getID()
				<< " and flags & "
				<< sqlLevel::F_AUTOINVITE
				<< " > 0"
				<< " and deleted = 0"
				<< ends;

		#ifdef LOG_SQL
			elog	<< "CHANINFO::sqlQuery> "
				<< autoInviteQuery.str().c_str()
				<< endl;
		#endif


		if( !bot->SQLDb->Exec(autoInviteQuery, true ) )
//	if( PGRES_TUPLES_OK != status )
			{

			elog	<< "CHANINFO> SQL Error: "
				<< bot->SQLDb->ErrorMessage()
				<< endl ;
			return  false;
			}
		if(bot->SQLDb->Tuples() > 0)
		{
			flagsSet+= "INVITE ";
		}


	}
	/* set 'NONE' if no flags */
	if (flagsSet.size() == 0)
		flagsSet = "NONE ";

	/* Keep details private. */
	if (theUser->getFlag(sqlUser::F_INVIS))
		{

		/* If they don't have * access, deny. */
		if( !((tmpUser) && bot->getAdminAccessLevel(tmpUser)) && (tmpUser != theUser))
			{
			bot->Notice(theClient,
				bot->getResponse(tmpUser,
					language::no_peeking,
					string("Unable to view user details (Invisible)")));

			/* Show flags even when invisible */
			bot->Notice(theClient,
				bot->getResponse(tmpUser, language::status_flags,
				string("Flags set: %s")).c_str(),
				flagsSet.c_str());

			/*
			 * Show the channels this guy owns to opers.
			 */

			if (theClient->isOper())
			{
				bot->outputChannelAccesses(theClient, theUser, tmpUser, 500);
			}

			return false;
			}
		}

	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::info_about,
			string("Information about: %s (%i)")).c_str(),
		theUser->getUserName().c_str(), theUser->getID());

	if (theUser->getID() == 1)
	{
		bot->Notice(theClient," - The one that was, the one that is, the one that will be.");
	}

	if (theUser->getID() == 42)
	{
		bot->Notice(theClient," - What do you mean you want to demolish the Undernet to make way for a new"
		" hyperspace expressway?");
	}

	/*
	 * Loop over all people we might be logged in as.
	 */

	bot->Notice(theClient, "Currently logged on via:");
	int aCount = 0;

	for( sqlUser::networkClientListType::iterator ptr = theUser->networkClientList.begin() ;
		ptr != theUser->networkClientList.end() ; ++ptr )
		{
			if ((tmpadminLevel > 0 || theUser->getFlag(sqlUser::F_OPER)) && adminAccess < 800)
			{
				bot->Notice(theClient, "  " + (*ptr)->getNickName() + "!" +
					(*ptr)->getUserName() + "@" +
					(*ptr)->getAccount() + (*ptr)->getHiddenHostSuffix());
			} else {
				bot->Notice(theClient, "  " + (*ptr)->getNickUserHost());
			}
			aCount++;
		}

	if (!aCount) bot->Notice(theClient, "  OFFLINE");

	bot->Notice(theClient,
		bot->getResponse(tmpUser, language::status_flags,
			string("Flags set: %s")).c_str(),
			flagsSet.c_str());

	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::last_seen,
			string("Last Seen: %s")).c_str(),
		bot->prettyDuration(theUser->getLastSeen()).c_str());

	if(adminAccess)
	{
	/*
	 * Show admins some more details about the user.
	 */
	unsigned int theTime;
	string userComments = theUser->getLastEvent(sqlUser::EV_COMMENT, theTime);

	if (!userComments.empty() && (userComments[0] != ' '))
		{
			bot->Notice(theClient,"\002Admin Comment\002: %s ago (%s)", bot->prettyDuration(theTime).c_str(),
				userComments.c_str());
		}

	if (theUser->getFlag(sqlUser::F_FRAUD))
		{
		bot->Notice(theClient, "\002** This account has been tagged as being used in a fraudulent channel application **\002");
		}

	if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
		{
		/*
		 * Perform a lookup to get the last SUSPEND event from the userlog.
		 */
		unsigned int theTime;
		string reason = theUser->getLastEvent(sqlUser::EV_SUSPEND, theTime);

		/* alter the reason for web-based suspensions */
		if (adminAccess < 800)
		{
			/* check if this is a web-based suspension */
			if ((reason.size() > 7) && (reason.substr(0,7)=="[Web]: "))
			{
				/* tokenize the string and reconstruct it without 2nd token */
				StringTokenizer rst(reason);
				reason = rst[0];
				reason += " ";
				reason += rst.assemble(2);
			}
		}

		bot->Notice(theClient, "Account suspended %s ago, Reason: %s", bot->prettyDuration(theTime).c_str(),
			reason.c_str());
		} else
		{
		/*
		 *  Maybe they where unsuspended recently..
		 */

		unsigned int theTime;
		string reason = theUser->getLastEvent(sqlUser::EV_UNSUSPEND, theTime);
		if (!reason.empty())
			{
				/* alter the reason for web-based suspensions */
				if (adminAccess < 800)
				{
					/* check if this is a web-based suspension */
					if ((reason.size() > 7) && (reason.substr(0,7)=="[Web]: "))
					{
						/* tokenize the string and reconstruct it without 2nd token */
						StringTokenizer rst(reason);
						reason = rst[0];
						reason += " ";
						reason += rst.assemble(2);
					}
				}
				bot->Notice(theClient, "Account was unsuspended %s ago %s",
					bot->prettyDuration(theTime).c_str(),
					reason.c_str());
			}
		}
	}

	/*
	 * Run a query to see what channels this user has access on. :)
	 * Only show to those with admin access, or the actual user.
	 */

	if( adminAccess || (tmpUser == theUser) )
		{
		bot->Notice(theClient, "EMail: %s",
			theUser->getEmail().c_str());

		if ((tmpUser != theUser) && ((tmpadminLevel > 0 || theUser->getFlag(sqlUser::F_OPER))) && (adminAccess < 800))
		{
			bot->Notice(theClient, "Last Hostmask: Not Available");
		} else {
			bot->Notice(theClient, "Last Hostmask: %s",
				theUser->getLastHostMask().c_str());
			//Show ip only to admins
			if(adminAccess > 0) 
				{
				bot->Notice(theClient, "Last IP: %s",
					theUser->getLastIP().c_str());			
				}
		}

#ifdef USE_NOTES
		if(theUser->getFlag(sqlUser::F_NONOTES))
			{
			bot->Notice(theClient, "%s doesn't accept Notes.", theUser->getUserName().c_str());
			} else
			{
			bot->Notice(theClient, "%s happily accepts Notes.", theUser->getUserName().c_str());
			}
#endif

		bot->outputChannelAccesses(theClient, theUser, tmpUser, 0);

		}

	/*
	 * Show the channels this guy owns to opers.
	 */

	if (theClient->isOper() && !adminAccess && (tmpUser != theUser))
	{
		bot->outputChannelAccesses(theClient, theUser, tmpUser, 500);
	}

	
	return true;
}

sqlUser* theUser = bot->isAuthed(theClient, false);
sqlChannel* theChan = bot->getChannelRecord(st[1]);
if( !theChan )
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::chan_not_reg,
			string("The channel %s is not registered")).c_str(),
		st[1].c_str());
	return true;
	}

/*
 * Receiving all the level 500's of the channel through a sql query.
 * The description and url, are received from the cache. --Plexus
 */

stringstream theQuery;
theQuery	<< queryHeader
		<< queryString
		<< "AND levels.channel_id = "
		<< theChan->getID()
		<< ends;

#ifdef LOG_SQL
	elog	<< "CHANINFO::sqlQuery> "
		<< theQuery.str().c_str()
		<< endl;
#endif

bot->Notice(theClient,
	bot->getResponse(theUser,
		language::reg_by,
		string("%s is registered by:")).c_str(),
	st[1].c_str());

if( bot->SQLDb->Exec(theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	for(unsigned int i = 0; i < bot->SQLDb->Tuples(); i++)
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::last_seen_info,
				string("%s - last seen: %s ago")).c_str(),
			bot->SQLDb->GetValue(i, 1).c_str(),
			bot->prettyDuration(atoi(bot->SQLDb->GetValue(i, 3))).c_str());
		} // for()
	}

if(theChan->getFlag(sqlChannel::F_SPECIAL) && !adminAccess) return true;

if( !theChan->getDescription().empty() )
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::desc,
			string("Desc: %s")).c_str(),
		theChan->getDescription().c_str());
	}

if ((adminAccess > 0) && (theChan->getFlag(sqlChannel::F_SUSPEND)))
{
	string suspender = "";
	string suspendreason = "";
	unsigned int suspendts = 0;
	stringstream queryString;

	queryString	<< "SELECT message,ts FROM channellog WHERE "
			<< "channelid = "
			<< theChan->getID()
			<< " AND event = "
			<< sqlChannel::EV_SUSPEND
			<< " AND ts <= "
			<< bot->currentTime()
			<< " ORDER BY ts DESC LIMIT 1"
			<< ends;

#ifdef LOG_SQL
	elog	<< "cservice::CHANINFOCommand> "
		<< queryString.str().c_str()
		<< endl;
#endif

	if (bot->SQLDb->Exec(queryString, true))
	{
		if (bot->SQLDb->Tuples() > 0)
		{
			string tempreason = bot->SQLDb->GetValue(0, 0);
			StringTokenizer suspendst(tempreason);
			if (suspendst.size() > 5)
			{
				if ( (suspendst[2].substr(0,1)=="(") &&
				(suspendst[2].substr(suspendst[2].size()-1,1)==")"))
					suspender = suspendst[2].substr(1,
						suspendst[2].size()-2);
				else
					suspender = suspendst[2];
				suspendreason = suspendst.assemble(5);
			}
			suspendts = atoi(bot->SQLDb->GetValue(0, 1));
		}
	}

	if (suspendreason != "")
	{
		bot->Notice(theClient, "Channel suspended %s ago by %s, Reason: %s",
			bot->prettyDuration(suspendts).c_str(),
			suspender.c_str(),
			suspendreason.c_str());
	}
}

if( !theChan->getComment().empty() && adminAccess )
	{
	if((tmpUser) && bot->getAdminAccessLevel(tmpUser))
		{
		bot->Notice(theClient, "Comments: %s",
			theChan->getComment().c_str());
		}
	}

if( !theChan->getKeywords().empty() )
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::keywords,
			string("Keywords: %s")).c_str(),
		theChan->getKeywords().c_str());
	}

if( !theChan->getURL().empty() )
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::url,
			string("URL: %s")).c_str(),
		theChan->getURL().c_str());
	}

if (theChan->getFlag(sqlChannel::F_TEMP))
	{
	bot->Notice(theClient, "\002This channel has a temporary manager.\002");
	}

return true;
}

} // namespace gnuworld.
