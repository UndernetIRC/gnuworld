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
 * $Id: CHANINFOCommand.cc,v 1.11 2001/01/30 00:12:16 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"
#include	"libpq++.h"
 
const char CHANINFOCommand_cc_rcsId[] = "$Id: CHANINFOCommand.cc,v 1.11 2001/01/30 00:12:16 gte Exp $" ;
 
namespace gnuworld
{

using std::string ;

static const char* queryHeader = "SELECT channels.name,users.user_name,levels.access,users_lastseen.last_seen FROM levels,channels,users,users_lastseen ";
static const char* queryString = "WHERE levels.channel_id=channels.id AND users.id=users_lastseen.user_id AND levels.access = 500 AND levels.user_id = users.id ";
 
bool CHANINFOCommand::Exec( iClient* theClient, const string& Message )
{ 
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	} 

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
		bot->Notice(theClient, "The user %s doesn't appear to be registered.",
			st[1].c_str());
		return true;
		}
 
	/* Keep details private. */
	sqlUser* tmpUser = bot->isAuthed(theClient, false);

	if (theUser->getFlag(sqlUser::F_INVIS))
		{
			
		/* If they don't have * access or are opered, deny. */ 
		if( !((tmpUser) && bot->getAdminAccessLevel(tmpUser)) && !(theClient->isOper()))
			{
			bot->Notice(theClient, "Unable to view user details (Invisible)");
			return false;
			}
		}

	bot->Notice(theClient, "Information about: %s (%i)",
		theUser->getUserName().c_str(), theUser->getID());

	iClient* targetClient = theUser->isAuthed();
	string loggedOn = targetClient ?
	targetClient->getNickUserHost() : "Offline";

	bot->Notice(theClient, "Currently logged on via: %s",
		loggedOn.c_str());
	if(theUser->getUrl() != "")
	{
		bot->Notice(theClient, "URL: %s",
			theUser->getUrl().c_str());
	}
	bot->Notice(theClient, "Language: %i",
		theUser->getLanguageId()); 
	bot->Notice(theClient, "Last Seen: %s",
		bot->prettyDuration(theUser->getLastSeen()).c_str()); 

	/*
	 * Run a query to see what channels this user has access on. :)
	 * Only show to those with admin access, opers, or the actual user.
	 */

	if( ((tmpUser) && bot->getAdminAccessLevel(tmpUser)) || (theClient->isOper()) || (tmpUser == theUser) )
	{
		strstream channelsQuery;
		string channelList = ""; 
	
		channelsQuery << "SELECT channels.name,levels.access FROM levels,channels "
				<< "WHERE levels.channel_id = channels.id AND levels.user_id = "
				<< theUser->getID() << " ORDER BY levels.access DESC"
				<< ends;
		
		elog << "CHANINFO::sqlQuery> " << channelsQuery.str() << endl;
	
		string chanName = "";
		string chanAccess ="";
		if( PGRES_TUPLES_OK == bot->SQLDb->Exec(channelsQuery.str()) )
			{
			for(int i = 0; i < bot->SQLDb->Tuples(); i++)
				{ 
					chanName = bot->SQLDb->GetValue(i,0);
					chanAccess = bot->SQLDb->GetValue(i,1);
					// 4 for 2 spaces, 2 brackets + comma.
					if ((channelList.size() + chanName.size() + chanAccess.size() +5) >= 500)
					{
						bot->Notice(theClient, "Channels: %s", channelList.c_str());
						channelList = "";
					}
						
					if (channelList.size() != 0) channelList += ", ";
					channelList += chanName; 
					channelList += " (";
					channelList += chanAccess;
					channelList +=  ")";
				} // for()
			}
	 
		bot->Notice(theClient, "Channels: %s", channelList.c_str());
	 
		delete[] channelsQuery.str() ; 

	}
	return true;
} 

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if( !theChan ) 
	{
	bot->Notice(theClient, "The channel %s is not registered",
		st[1].c_str());
	return true;
	}

/*
 * Receiving all the level 500's of the channel through a sql query.
 * The description and url, are received from the cache. --Plexus
 */

strstream theQuery;
theQuery	<< queryHeader << queryString
		<< "AND levels.channel_id = "
		<< theChan->getID()
		<< ends;

elog << "CHANINFO::sqlQuery> " << theQuery.str() << endl;
        
bot->Notice(theClient, "%s is registered by:",
	st[1].c_str());

ExecStatusType status = bot->SQLDb->Exec(theQuery.str()) ;
if( PGRES_TUPLES_OK == status )
	{
	for(int i = 0; i < bot->SQLDb->Tuples(); i++)
		{
		bot->Notice(theClient, "%s - last seen: %s ago",
			bot->SQLDb->GetValue(i, 1),
			bot->prettyDuration(atoi(bot->SQLDb->GetValue(i, 3))).c_str());
		} // for()
	}

if( !theChan->getDescription().empty() )
	{
	bot->Notice(theClient, "Desc: %s",
		theChan->getDescription().c_str());
	}

if( !theChan->getURL().empty() )
	{
	bot->Notice(theClient, "URL: %s",
		theChan->getURL().c_str());
	}

delete[] theQuery.str() ;
return true ; 
} 

} // namespace gnuworld.
