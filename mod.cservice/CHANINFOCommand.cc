/*
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
 * $Id: CHANINFOCommand.cc,v 1.45 2002/04/09 15:46:20 gte Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"libpq++.h"
#include	"cservice_config.h"

const char CHANINFOCommand_cc_rcsId[] = "$Id: CHANINFOCommand.cc,v 1.45 2002/04/09 15:46:20 gte Exp $" ;

namespace gnuworld
{
using std::string ;

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

sqlUser* tmpUser = bot->isAuthed(theClient, false);

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
			bot->Notice(theClient, "  " + (*ptr)->getNickUserHost());
			aCount++;
		}

	if (!aCount) bot->Notice(theClient, "  OFFLINE");

	if( !theUser->getUrl().empty() )
		{
		bot->Notice(theClient,
			bot->getResponse(tmpUser,
				language::url,
				string("URL: %s")).c_str(),
			theUser->getUrl().c_str());
		}

	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::lang,
			string("Language: %i")).c_str(),
		theUser->getLanguageId());

	if (theUser->getFlag(sqlUser::F_INVIS))
		{
		bot->Notice(theClient, "INVISIBLE is On");
		} else
		{
		bot->Notice(theClient, "INVISIBLE is Off");
		}

	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::last_seen,
			string("Last Seen: %s")).c_str(),
		bot->prettyDuration(theUser->getLastSeen()).c_str());

	if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
		{
		bot->Notice(theClient, "\002** This account has been suspended **\002");
		}

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
			bot->Notice(theClient, "Account was unsuspended %s ago%s", bot->prettyDuration(theTime).c_str(),
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

		bot->Notice(theClient, "Last Hostmask: %s",
			theUser->getLastHostMask().c_str());

		bot->Notice(theClient, "Max Logins: %i",
			theUser->getMaxLogins());

#ifdef USE_NOTES
		if(theUser->getFlag(sqlUser::F_NONOTES))
			{
			bot->Notice(theClient, "%s doesn't accept Notes.", theUser->getUserName().c_str());
			} else
			{
			bot->Notice(theClient, "%s happily accepts Notes.", theUser->getUserName().c_str());
			}
#endif

		strstream channelsQuery;
		string channelList ;

		channelsQuery	<< "SELECT channels.name,levels.access FROM levels,channels "
				<< "WHERE levels.channel_id = channels.id AND channels.registered_ts <> 0 AND levels.user_id = "
				<< theUser->getID()
				<< " ORDER BY levels.access DESC"
				<< ends;

		#ifdef LOG_SQL
			elog	<< "CHANINFO::sqlQuery> "
				<< channelsQuery.str()
				<< endl;
		#endif

		string chanName ;
		string chanAccess ;

		ExecStatusType status =
			bot->SQLDb->Exec(channelsQuery.str()) ;
		delete[] channelsQuery.str() ;

		if( PGRES_TUPLES_OK != status )
			{
			bot->Notice( theClient,
				"Internal error: SQL failed" ) ;

			elog	<< "CHANINFO> SQL Error: "
				<< bot->SQLDb->ErrorMessage()
				<< endl ;
			return false ;
			}

		for(int i = 0; i < bot->SQLDb->Tuples(); i++)
			{
			chanName = bot->SQLDb->GetValue(i,0);
			chanAccess = bot->SQLDb->GetValue(i,1);
			// 4 for 2 spaces, 2 brackets + comma.
			if ((channelList.size() + chanName.size() + chanAccess.size() +5) >= 450)
				{
				bot->Notice(theClient,
					bot->getResponse(tmpUser,
						language::channels,
						string("Channels: %s")).c_str(),
					channelList.c_str());
				channelList.erase( channelList.begin(),
					channelList.end() ) ;
				}

			if (channelList.size() != 0)
					{
					channelList += ", ";
					}
			channelList += chanName;
			channelList += " (";
			channelList += chanAccess;
			channelList +=  ")";
			} // for()

		bot->Notice(theClient,
			bot->getResponse(tmpUser,
				language::channels,
				string("Channels: %s")).c_str(),
			channelList.c_str());
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

strstream theQuery;
theQuery	<< queryHeader
		<< queryString
		<< "AND levels.channel_id = "
		<< theChan->getID()
		<< ends;

#ifdef LOG_SQL
	elog	<< "CHANINFO::sqlQuery> "
		<< theQuery.str()
		<< endl;
#endif

bot->Notice(theClient,
	bot->getResponse(theUser,
		language::reg_by,
		string("%s is registered by:")).c_str(),
	st[1].c_str());

ExecStatusType status = bot->SQLDb->Exec(theQuery.str()) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK == status )
	{
	for(int i = 0; i < bot->SQLDb->Tuples(); i++)
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::last_seen_info,
				string("%s - last seen: %s ago")).c_str(),
			bot->SQLDb->GetValue(i, 1),
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
