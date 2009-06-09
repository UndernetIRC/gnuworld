/**
 * LOGINCommand.cc
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
 * $Id: LOGINCommand.cc,v 1.67 2009/06/09 15:40:29 mrbean_ Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>
#include	<iomanip>
#include	<inttypes.h>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"md5hash.h"
#include	"responses.h"
#include	"levels.h"
#include	"networkData.h"
#include	"cservice_config.h"
#include	"Network.h"
#include	"ip.h"

const char LOGINCommand_cc_rcsId[] = "$Id: LOGINCommand.cc,v 1.67 2009/06/09 15:40:29 mrbean_ Exp $" ;

namespace gnuworld
{
struct autoOpData {
	unsigned int channel_id;
	unsigned int flags;
	time_t suspend_expires;
} aOp;

using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;
using namespace gnuworld;

bool LOGINCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.LOGIN");

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

/*
 * Are we allowing logins yet?
 */
unsigned int useLoginDelay = bot->getConfigVar("USE_LOGIN_DELAY")->asInt();
unsigned int loginTime = bot->getUplink()->getStartTime() + bot->loginDelay;
if ( (useLoginDelay == 1) && (loginTime >= (unsigned int)bot->currentTime()) )
	{
	bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (Unable "
		"to login during reconnection, please try again in "
		"%i seconds)",
		st[1].c_str(), (loginTime - bot->currentTime()));
	return false;
	}

/*
 * Check theClient isn't already logged in, if so, tell
 * them they shouldn't be.
 */

sqlUser* tmpUser = bot->isAuthed(theClient, false);
if (tmpUser)
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser, language::already_authed).c_str(),
		tmpUser->getUserName().c_str());
	return false;
	}

/*
 * Check if this client has exceeded their max logins
 */

unsigned int maxFailedLogins = bot->getConfigVar("MAX_FAILED_LOGINS")->asInt();
unsigned int failedLogins = bot->getFailedLogins(theClient);
if ((maxFailedLogins > 0) && (failedLogins >= maxFailedLogins))
{
	/* exceeded maximum failed logins */
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
		language::max_failed_logins,
		string("AUTHENTICATION FAILED as %s (Exceeded maximum login failures for this session)")).c_str(),
		st[1].c_str());
	return false;
}

/*
 * Find the user record, confirm authorisation and attach the record
 * to this client.
 */

if(st[1][0] == '#')
	{
	bot->setFailedLogins(theClient, failedLogins+1);
	bot->Notice(theClient, "AUTHENTICATION FAILED as %s.", st[1].c_str());
	return false;
	}

// TODO: Force a refresh of the user's info from the db
sqlUser* theUser = bot->getUserRecord(st[1]);
if( !theUser )
	{
	bot->setFailedLogins(theClient, failedLogins+1);
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::not_registered,
			string("AUTHENTICATION FAILED as %s.")).c_str(),
		st[1].c_str());
	return false;
	}

if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
	bot->setFailedLogins(theClient, failedLogins+1);
	bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (Suspended)",
	st[1].c_str());
	return false;
	}

/*
 * Check password, if its wrong, bye bye.
 */

unsigned int max_failed_logins = bot->getConfigVar("FAILED_LOGINS")->asInt();
unsigned int failed_login_rate = bot->getConfigVar("FAILED_LOGINS_RATE")->asInt();

/* if it's not configured, default to every 15 minutes */
if (failed_login_rate==0)
	failed_login_rate = 900;

if (!bot->isPasswordRight(theUser, st.assemble(2)))
	{
	bot->setFailedLogins(theClient, failedLogins+1);
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::auth_failed,
			string("AUTHENTICATION FAILED as %s.")).c_str(),
		theUser->getUserName().c_str());
	/* increment failed logins counter */
	theUser->incFailedLogins();
	if ((max_failed_logins > 0) && (theUser->getFailedLogins() > max_failed_logins) &&
		(theUser->getLastFailedLoginTS() < (time(NULL) - failed_login_rate)))
	{
		/* we have exceeded our maximum - alert relay channel */
		/* work out a checksum for the password.  Yes, I could have
		 * just used a checksum of the original password, but this
		 * means it's harder to 'fool' the check digit with a real
		 * password - create MD5 from original salt stored */
		unsigned char	checksum;
		md5		hash;
		md5Digest	digest;

		if (theUser->getPassword().size() < 9)
		{
			checksum = 0;
		} else {
			string salt = theUser->getPassword().substr(0, 8);
			string guess = salt + st.assemble(2);

			hash.update( (const unsigned char *)guess.c_str(), guess.size() );
			hash.report( digest );

			checksum = 0;
			for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++)
			{
				/* add ascii value to check digit */
				checksum += digest[i];
			}
		}

		theUser->setLastFailedLoginTS(time(NULL));
		bot->logPrivAdminMessage("%d failed logins for %s (last attempt by %s, checksum %d).",
			theUser->getFailedLogins(),
			theUser->getUserName().c_str(),
			theClient->getRealNickUserHost().c_str(),
			checksum);
	}
	return false;
	}

/*
 * Check if this is a privileged user, if so check against IP restrictions
 */
if ((bot->getAdminAccessLevel(theUser, true) > 0) && (!theUser->getFlag(sqlUser::F_ALUMNI)))
{
	/* ok, they have "*" access (excluding alumni's) */
	if (!bot->checkIPR(theClient, theUser))
	{
		bot->setFailedLogins(theClient, failedLogins+1);
		bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (IPR)",  
			st[1].c_str());
		/* notify the relay channel */
		bot->logAdminMessage("%s (%s) failed IPR check.",
			theClient->getNickName().c_str(),
			st[1].c_str());
		/* increment failed logins counter */
		theUser->incFailedLogins();
		if ((max_failed_logins > 0) && (theUser->getFailedLogins() > max_failed_logins) &&
			(theUser->getLastFailedLoginTS() < (time(NULL) - failed_login_rate)))
		{
			/* we have exceeded our maximum - alert relay channel */
			theUser->setLastFailedLoginTS(time(NULL));
			bot->logPrivAdminMessage("%d failed logins for %s (last attempt by %s).",
				theUser->getFailedLogins(),
				theUser->getUserName().c_str(),
				theClient->getRealNickUserHost().c_str());
		}
		return false;
	}
}

/*
 * Don't exceed MAXLOGINS.
 */

bool iploginallow = false;
if(theUser->networkClientList.size() + 1 > theUser->getMaxLogins())
	{
	/* They have exceeded their maxlogins setting, but check if they
	   are allowed to login from the same IP - only applies if their
	   maxlogins is set to ONE */
	uint32_t iplogins = bot->getConfigVar("LOGINS_FROM_SAME_IP")->asInt();
	uint32_t iploginident = bot->getConfigVar("LOGINS_FROM_SAME_IP_AND_IDENT")->asInt();
	if ((theUser->getMaxLogins() == 1) && (iplogins > 1))
	{
		/* ok, we're using the multi-logins feature (0=disabled) */
		if (theUser->networkClientList.size() + 1 <= iplogins)
		{
			/* Check their IP from previous session against
			   current IP.  If it matches, allow the login.
			   As this only applies if their maxlogin is 1, we
			   know there is only 1 entry in their clientlist */
			if (theClient->getIP() == theUser->networkClientList.front()->getIP())
			{
				if (iploginident==1)
				{
					/* need to check ident here */
					string oldident = theUser->networkClientList.front()->getUserName();
					string newident = theClient->getUserName();
					if ((oldident[0]=='~') || (oldident==newident))
					{
						/* idents match (or they are unidented) - allow this login */
						iploginallow = true;
					}
				} else {
					/* don't need to check ident, this login is allowed */
					iploginallow = true;
				}
			}
		}
	}
	if (!iploginallow)
	{
		bot->setFailedLogins(theClient, failedLogins+1);
		bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (Maximum "
			"concurrent logins exceeded).",
			theUser->getUserName().c_str());

		string clientList;
		for( sqlUser::networkClientListType::iterator ptr = theUser->networkClientList.begin() ;
			ptr != theUser->networkClientList.end() ; )
			{
			clientList += (*ptr)->getNickUserHost();
			++ptr;
			if (ptr != theUser->networkClientList.end())
				{
				clientList += ", ";
				}
			} // for()

		bot->Notice(theClient, "Current Sessions: %s", clientList.c_str());
		return false;
		} // for()
	}

/*
 * If this user account is already authed against, send a notice to the other
 * users warning them that someone else has logged in too.
 */
if(theUser->isAuthed())
	{
	bot->noticeAllAuthedClients(theUser,
	"%s has just authenticated as you (%s). "
	"If this is not you, your account may have been compromised. "
	"If you wish to suspend all your access as a precautionary measure, "
	"type '\002/msg %s@%s suspendme <password>'\002 and contact a "
	"CService representative to resolve the problem."
	" \002** Note: You will NOT be able to use your account after "
	"you issue this command **\002",
		theClient->getNickUserHost().c_str(),
		theUser->getUserName().c_str(),
		bot->getNickName().c_str(),
		bot-> getUplinkName().c_str());
	}

/* check to see if we have a last seen time (bug workaround) - if not, make one */

stringstream queryString;
queryString	<< "SELECT last_seen FROM users_lastseen WHERE user_id="
		<< theUser->getID()
		<< ends;
#ifdef LOG_SQL
	elog	<< "LOGIN::sqlQuery> "
		<< queryString.str().c_str()
		<< endl;
#endif

if( bot->SQLDb->Exec(queryString, true ) )
//if (PGRES_TUPLES_OK == status)
{
	if (bot->SQLDb->Tuples() < 1)
	{
		/* no rows returned - create a dummy record that will be updated
		 * by setLastSeen after this loop
		 */
		stringstream updateQuery;
		updateQuery	<< "INSERT INTO users_lastseen (user_id,"
				<< "last_seen,last_updated) VALUES("
				<< theUser->getID()
				<< ",now()::abstime::int4,now()::abstime::int4)"
				<< ends;

#ifdef LOG_SQL
		elog	<< "LOGIN::sqlQuery> "
			<< updateQuery.str().c_str()
			<< endl;
#endif
		bot->SQLDb->Exec(updateQuery);
	}
}
/* update their details */
theUser->setLastSeen(bot->currentTime(), theClient->getNickUserHost(),
	theClient->getNickName() + "!" + theClient->getUserName() + "@" + xIP( theClient->getIP()).GetNumericIP());
theUser->setFlag(sqlUser::F_LOGGEDIN);
theUser->addAuthedClient(theClient);

networkData* newData =
	static_cast< networkData* >( theClient->getCustomData(bot) ) ;
if( NULL == newData )
	{
	bot->Notice( theClient,
		"Internal error." ) ;
	elog	<< "LOGINCommand> newData is NULL for: "
		<< theClient
		<< endl ;
	return false ;
	}

// Pointer back to the sqlUser from this iClient.
newData->currentUser = theUser;

bot->Notice(theClient,
	bot->getResponse(theUser, language::auth_success).c_str(),
	theUser->getUserName().c_str());

/* set failed logins counter back to zero after alerting the user (if required) */
if (bot->getConfigVar("ALERT_FAILED_LOGINS")->asInt()==1 &&
	theUser->getFailedLogins() > 0)
{
	/* ok, alert the user */
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::auth_failed_logins,
			string("There were %d failed login attempts since your last successful login.")).c_str(),
			theUser->getFailedLogins());
}
theUser->setFailedLogins(0);
theUser->setLastFailedLoginTS(0);

int tmpLevel = bot->getAdminAccessLevel(theUser);
if (tmpLevel > 0)
	{
		/* this is a privileged user, send a notice to _info */
		if (tmpLevel < 900)
		{
			/* regular admin, report to normal relay channel */
			bot->logAdminMessage("%s (%s) has authenticated (level %d)",
				theClient->getNickName().c_str(),
				theUser->getUserName().c_str(),
				tmpLevel);
		} else {
			/* if this is a high level admin, report to privileged relay channel */
			bot->logPrivAdminMessage("%s (%s) has authenticated (level %d)",
				theClient->getNickName().c_str(),
				theUser->getUserName().c_str(),
				tmpLevel);
		}
	} else {
	string greeting = bot->getResponse(theUser, language::greeting);
	if (!greeting.empty())
		{
		bot->Notice(theClient, greeting.c_str());
		}
	}

/*
 * Send out AC token onto the network.
 * The AC Token (Account) is read by ircu2.10.11 and any other
 * service on the net that cares and added to the Client structure.
 * Format:
 * [Source Server] AC [Authenticated User's Numeric] [Authenticated UserName]
 * Eg: AX AC APAFD gte
 */

server->UserLogin(theClient, theUser->getUserName(), bot);

/*
 * If the user account has been suspended, make sure they don't get
 * auto-opped.
 */

if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
	bot->Notice(theClient,
		"..however your account has been suspended by a CService administrator."
		" You will be unable to use any channel access you may have.");
	return true;
	}

/*
 * The fun part! For all channels this user has access on, and has
 * AUTOP set, and isn't already op'd on - do the deed.
 */

stringstream theQuery;
theQuery	<< "SELECT channel_id,flags,suspend_expires FROM "
		<< "levels WHERE user_id = "
		<< theUser->getID()
		<< ends;

#ifdef LOG_SQL
	elog	<< "LOGIN::sqlQuery> "
		<< theQuery.str().c_str()
		<< endl;
#endif

if( !bot->SQLDb->Exec(theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "LOGIN> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

typedef vector < autoOpData > autoOpVectorType;
autoOpVectorType autoOpVector;

for(unsigned int i = 0; i < bot->SQLDb->Tuples(); i++)
	{
	autoOpData current;

	current.channel_id = atoi(bot->SQLDb->GetValue(i, 0));
	current.flags = atoi(bot->SQLDb->GetValue(i, 1));
	current.suspend_expires = atoi(bot->SQLDb->GetValue(i, 2));

	autoOpVector.push_back( autoOpVectorType::value_type(current) );
	}

for (autoOpVectorType::const_iterator resultPtr = autoOpVector.begin();
	resultPtr != autoOpVector.end(); ++resultPtr)
	{

	/* If the autoop flag isn't set in this record */
	if (!(resultPtr->flags & sqlLevel::F_AUTOOP) &&
		!(resultPtr->flags & sqlLevel::F_AUTOVOICE) &&
		!(resultPtr->flags & sqlLevel::F_AUTOINVITE))
		{
		continue;
		}

	sqlChannel* theChan = bot->getChannelRecord(resultPtr->channel_id);
	if (!theChan)
		{
		continue;
		}

	/*
	 * Make sure the channel isn't suspended..
	 */

	if (theChan->getFlag(sqlChannel::F_SUSPEND))
		{
		continue;
		}


	/*
	 * Check if they're already opped.
	 */

	Channel* netChan = Network->findChannel(theChan->getName());
	if (!netChan)
		{
		continue;
		}


	/*
	 * Don't attempt to op/invite if we're not in the channel, or not op'd.
	 */

	ChannelUser* tmpBotUser = netChan->findUser(bot->getInstance());
	if (!tmpBotUser)
		{
		continue;
		}

	if (!theChan->getInChan() || !tmpBotUser->getMode(ChannelUser::MODE_O))
		{
		continue;
		}

	/*
	 *  Would probably be wise to check they're not suspended too :)
	 *  (*smack* Ace)
	 */

	if(resultPtr->suspend_expires > bot->currentTime() )
		{
		continue;
		}

	ChannelUser* tmpChanUser = netChan->findUser(theClient) ;
	if(!tmpChanUser)
		{
		//The user is not in the channel, lets see if their autoinvite is on and act upon it
		if (resultPtr->flags & sqlLevel::F_AUTOINVITE) 
			{
			int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
			if(level >= level::invite) 
				{
				bot->Invite(theClient,netChan);
				}
			}
		continue;
		}

	/*
	 * Check if the channel is NOOP.
	 * N.B: If the channel is strictop, we op them.
	 * They've just logged in! :P
	 */

	if(theChan->getFlag(sqlChannel::F_NOOP))
		{
		continue;
		}

	/*
	 * Check they aren't banned < 75 in the chan.
	 */

	sqlBan* tmpBan = bot->isBannedOnChan(theChan, theClient);
	if( tmpBan && (tmpBan->getLevel() < 75) )
		{
		continue;
		}

	/*
 	 *  If its AUTOOP, check for op's and do the deed.
	 *  Otherwise, its just AUTOVOICE :)
	 */

	if (resultPtr->flags & sqlLevel::F_AUTOOP)
		{
		if(!tmpChanUser->getMode(ChannelUser::MODE_O))
			{
			bot->Op(netChan, theClient);
			}
		}
	else if (resultPtr->flags & sqlLevel::F_AUTOVOICE)
		{
		if(!tmpChanUser->getMode(ChannelUser::MODE_V))
			{
			bot->Voice(netChan, theClient);
			}
		}

	}

/*
 *  And last but by no means least, see if we have been nominated as
 *  a supporter for a channel.
 */

stringstream supporterQuery;
supporterQuery	<< "SELECT channels.name FROM"
			<< " supporters,channels,pending WHERE"
			<< " supporters.channel_id = channels.id"
			<< " AND pending.channel_id = channels.id"
			<< " AND channels.registered_ts = 0"
			<< " AND supporters.support = '?'"
			<< " AND pending.status = 0"
			<< " AND user_id = "
			<< theUser->getID()
			<< ends;

#ifdef LOG_SQL
	elog	<< "LOGIN::sqlQuery> "
		<< supporterQuery.str().c_str()
		<< endl;
#endif

if( !bot->SQLDb->Exec(supporterQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "LOGIN> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}


for(unsigned int i = 0; i < bot->SQLDb->Tuples(); i++)
	{
	string channelName = bot->SQLDb->GetValue(i, 0);
	bot->Notice(theClient, "You have been named as a supporter in a "
		"new channel application for %s. You may visit the "
		"website to register your support or to make an "
		"objection. Alternatively, you can"
		" type '\002/msg %s support %s YES\002' or '\002/msg %s "
		"support %s NO\002' to confirm or deny your support.",
		channelName.c_str(),
		bot->getNickName().c_str(),
		channelName.c_str(),
		bot->getNickName().c_str(),
		channelName.c_str());
	}

/*
 * See if they have any notes.
 */

#ifdef USE_NOTES

if(!theUser->getFlag(sqlUser::F_NONOTES))
	{
	stringstream noteQuery;
	noteQuery	<< "SELECT message_id FROM notes "
			<< "WHERE user_id = "
			<< theUser->getID()
			<< ends;

#ifdef LOG_SQL
	elog	<< "LOGIN::sqlQuery> "
		<< noteQuery.str().c_str()
		<< endl;
#endif

	bot->SQLDb->Exec(noteQuery, true) ;

	unsigned int count = bot->SQLDb->Tuples();
	if(count)
		{
		bot->Notice(theClient, "You have %i note(s). To read "
			"them type /msg %s notes read all",
			count,
			bot->getNickName().c_str());
		}
	}

#endif

return true;
}

} // namespace gnuworld.

