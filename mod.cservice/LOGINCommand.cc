/* LOGINCommand.cc */

#include	<string>
#include	<iomanip.h>

#include	"md5hash.h" 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"responses.h" 
#include	"networkData.h"
#include	"cservice_config.h"
#include	"Network.h"

const char LOGINCommand_cc_rcsId[] = "$Id: LOGINCommand.cc,v 1.19 2001/06/21 23:23:56 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool LOGINCommand::Exec( iClient* theClient, const string& Message )
{
 
StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
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
	bot->Notice(theClient, "Sorry, I don't know who %s is.", st[1].c_str());
	return false;
}

// TODO: Force a refresh of the user's info from the db 
sqlUser* theUser = bot->getUserRecord(st[1]);
if( !theUser )
	{
	bot->Notice(theClient, 
		bot->getResponse(tmpUser,
			language::not_registered,
			string("Sorry, I don't know who %s is.")).c_str(), 
		st[1].c_str());
	return false;
	}

/*
 *  Compare password with MD5 hash stored in user record.
 */

// MD5 hash algorithm object.
md5	hash;

// MD5Digest algorithm object.
md5Digest digest;

string salt = theUser->getPassword().substr(0, 8);
string md5Part = theUser->getPassword().substr(8);
string guess = salt + st.assemble(2);

// Build a MD5 hash based on our salt + the guessed password.
hash.update( (const unsigned char *)guess.c_str(), guess.size() );
hash.report( digest );

// Convert the digest into an array of int's to output as hex for 
// comparison with the passwords generated by PHP.
int data[ MD5_DIGEST_LENGTH ] = { 0 } ;

for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
	{
	data[ii] = digest[ii];
	}

strstream output;
output << hex;
output.fill('0');

for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
	{
	output << setw(2) << data[ii];
	}
output << ends;

if(md5Part != output.str() ) // If the MD5 hash's don't match..
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::auth_failed,
			string("AUTHENTICATION FAILED as %s (Invalid Password).")).c_str(), 
		theUser->getUserName().c_str());

	delete[] output.str() ;
	return false;
	}

delete[] output.str() ;

/*
 *  Check this user isn't already spoken for..
 *  If someone HAS authenticated as this user, then deauth that other
 *  person.
 */

iClient* authTestUser = theUser->isAuthed();
if (authTestUser)
	{
	bot->Notice(authTestUser, 
		bot->getResponse(tmpUser,
			language::no_longer_auth,
			string("NOTICE: %s has now authenticated as %s, you are no longer authenticated.")).c_str(),
			theClient->getNickUserHost().c_str(),
			theUser->getUserName().c_str());

	networkData* tmpData =
		static_cast< networkData* >( authTestUser->getCustomData(bot) ) ;
	if( NULL == tmpData )
		{
		bot->Notice( authTestUser,
			"Internal error." ) ;
		elog	<< "LOGINCommand> tmpData is NULL for: "
			<< *authTestUser
			<< endl ;
		return false ;
		}

	// Remove the pointer from the iClient to the sqlUser.
	tmpData->currentUser = NULL;
	}
 
theUser->setLastSeen(bot->currentTime());
theUser->setFlag(sqlUser::F_LOGGEDIN);
theUser->networkClient = theClient; // Who is authed as this user. 

networkData* newData =
	static_cast< networkData* >( theClient->getCustomData(bot) ) ;
if( NULL == newData )
	{
	bot->Notice( authTestUser,
		"Internal error." ) ;
	elog	<< "LOGINCommand> newData is NULL for: "
		<< *authTestUser
		<< endl ;
	return false ;
	}

// Pointer back to the sqlUser from this iClient. 
newData->currentUser = theUser;

bot->Notice(theClient,
	bot->getResponse(theUser, language::auth_success).c_str(), 
	theUser->getUserName().c_str()); 

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
theQuery	<< "SELECT channel_id,flags FROM "
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
 
typedef vector < pair < int, int > > autoOpVectorType;
autoOpVectorType autoOpVector;

for(int i = 0; i < bot->SQLDb->Tuples(); i++)
	{ 
		int channel_id = atoi(bot->SQLDb->GetValue(i, 0));
		int flags = atoi(bot->SQLDb->GetValue(i, 1)); 

		autoOpVector.push_back(autoOpVectorType::value_type(
			make_pair(channel_id, flags)));
	}
 
for (autoOpVectorType::const_iterator resultPtr = autoOpVector.begin();
	resultPtr != autoOpVector.end(); ++resultPtr)
	{
 
	/* If the autoop flag isn't set in this record */
	if (!(resultPtr->second & sqlLevel::F_AUTOOP) &&
		!(resultPtr->second & sqlLevel::F_AUTOVOICE))
		{
		continue;
		}
 
	sqlChannel* theChan = bot->getChannelRecord(resultPtr->first);
	if (!theChan)
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
 	 *  If its AUTOOP, check for op's and do the deed.
	 *  Otherwise, its just AUTOVOICE :)
	 */

	if (resultPtr->second & sqlLevel::F_AUTOOP)
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
			<< " supporters,channels WHERE supporters.channel_id = channels.id"
			<< " AND channels.registered_ts = 0"
			<< " AND supporters.support = NULL"
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
		string channelName = bot->SQLDb->GetValue(i, 1);
		bot->Notice(theClient, "You have been named as a supporter in a new channel application"
			" for %s. Please visit the webpage to confirm your support, or register an objection with the application.",
			channelName.c_str());
	}
 
return true; 
} 

} // namespace gnuworld.
