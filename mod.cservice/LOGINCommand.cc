/* LOGINCommand.cc */

#include	<string>
#include	<iomanip.h>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"responses.h"
#include	"networkData.h"
#include	"cservice_config.h"
#include	"Network.h"

const char LOGINCommand_cc_rcsId[] = "$Id: LOGINCommand.cc,v 1.44 2002/04/01 22:02:22 gte Exp $" ;

namespace gnuworld
{
struct autoOpData {
	unsigned int channel_id;
	unsigned int flags;
	unsigned int suspend_expires;
} aOp;

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
unsigned int loginTime = bot->getUplink()->getStartTime() + bot->loginDelay;
if(loginTime >= (unsigned int)bot->currentTime())
{
	bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (Unable to login during reconnection, please try again in %i seconds)",
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
 * Find the user record, confirm authorisation and attach the record
 * to this client.
 */

if(st[1][0] == '#')
{
	bot->Notice(theClient, "AUTHENTICATION FAILED as %s.", st[1].c_str());
	return false;
}

// TODO: Force a refresh of the user's info from the db
sqlUser* theUser = bot->getUserRecord(st[1]);
if( !theUser )
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::not_registered,
			string("AUTHENTICATION FAILED as %s.")).c_str(),
		st[1].c_str());
	return false;
	}

if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
	bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (Suspended)",
	st[1].c_str());
	return false;
	}

/*
 * Check password, if its wrong, bye bye.
 */

if (!bot->isPasswordRight(theUser, st.assemble(2)))
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::auth_failed,
			string("AUTHENTICATION FAILED as %s.")).c_str(),
		theUser->getUserName().c_str());
	return false;
	}

/*
 * Don't exceed MAXLOGINS.
 */

if(theUser->networkClientList.size() + 1 > theUser->getMaxLogins())
{
	bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (Maximum concurrent logins exceeded).",
		theUser->getUserName().c_str());

	string clientList;
	for( sqlUser::networkClientListType::iterator ptr = theUser->networkClientList.begin() ;
		ptr != theUser->networkClientList.end() ; )
		{
			clientList += (*ptr)->getNickUserHost();
			++ptr;
			if (ptr != theUser->networkClientList.end()) clientList += ", ";
		}

	bot->Notice(theClient, "Current Sessions: %s", clientList.c_str());
	return false;
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
	"type '\002/msg %s@%s suspendme <password>'\002 and contact a CService representative to resolve the problem."
	" \002** Note: You will NOT be able to use your account after you issue this command **\002",
		theClient->getNickUserHost().c_str(), theUser->getUserName().c_str(),
		bot->getNickName().c_str(), bot-> getUplinkName().c_str());
}

theUser->setLastSeen(bot->currentTime(), theClient->getNickUserHost());
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

if(!bot->getAdminAccessLevel(theUser))
{
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
 * Eg: AXAAA AC APAFD gte
 */

#if 1
strstream ac;
ac	<< bot->getCharYY()
	<< " AC "
	<< theClient->getCharYYXXX()
	<< " " << theUser->getUserName()
	<< ends;
bot->Write( ac );
delete[] ac.str();
theClient->setAccount(theUser->getUserName());
#endif

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

strstream theQuery;
theQuery	<< "SELECT channel_id,flags,suspend_expires FROM "
			<< "levels WHERE user_id = "
			<< theUser->getID()
			<< ends;

#ifdef LOG_SQL
	elog	<< "LOGIN::sqlQuery> "
		<< theQuery.str()
		<< endl;
#endif

ExecStatusType status = bot->SQLDb->Exec(theQuery.str()) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "LOGIN> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

typedef vector < autoOpData > autoOpVectorType;
autoOpVectorType autoOpVector;

for(int i = 0; i < bot->SQLDb->Tuples(); i++)
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
		!(resultPtr->flags & sqlLevel::F_AUTOVOICE))
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
	 * Check if they're already opped.
	 */

	Channel* netChan = Network->findChannel(theChan->getName());
	if (!netChan)
		{
		continue;
		}

	ChannelUser* tmpChanUser = netChan->findUser(theClient) ;
	if(!tmpChanUser)
		{
		continue;
		}

	/*
	 * Don't attempt to op if we're not in the channel, or not op'd.
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

	if(resultPtr->suspend_expires > 0)
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
		else
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

strstream supporterQuery;
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
		<< supporterQuery.str()
		<< endl;
#endif

status = bot->SQLDb->Exec(supporterQuery.str()) ;
delete[] supporterQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "LOGIN> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}


for(int i = 0; i < bot->SQLDb->Tuples(); i++)
	{
		string channelName = bot->SQLDb->GetValue(i, 0);
		bot->Notice(theClient, "You have been named as a supporter in a new channel application"
			" for %s. You may visit the website to register your support or to make an objection. Alternatively, you can"
			" type '\002/msg X support %s YES\002' or '\002/msg X support %s NO\002' to confirm or deny your support.",
			channelName.c_str(), channelName.c_str(), channelName.c_str());
	}

/*
 * See if they have any notes.
 */

if(!theUser->getFlag(sqlUser::F_NONOTES))
	{
	strstream noteQuery;
	noteQuery	<< "SELECT message_id FROM notes "
				<< "WHERE user_id = "
				<< theUser->getID()
				<< ends;

#ifdef LOG_SQL
	elog	<< "LOGIN::sqlQuery> "
		<< noteQuery.str()
		<< endl;
#endif

	status = bot->SQLDb->Exec(noteQuery.str()) ;
	delete[] noteQuery.str() ;

	unsigned int count = bot->SQLDb->Tuples();
	if(count) bot->Notice(theClient, "You have %i note(s). To read them type /msg %s notes read all",
		count, bot->getNickName().c_str());
	}

return true;
}

} // namespace gnuworld.

