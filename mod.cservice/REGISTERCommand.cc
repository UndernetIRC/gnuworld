/* 
 * REGISTERCommand.cc 
 *
 * 26/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Registers a channel.
 *
 * Caveats: None
 *
 * $Id: REGISTERCommand.cc,v 1.10 2001/02/20 22:31:04 gte Exp $
 */
 
#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"
#include	"Network.h"
#include	"responses.h"

const char REGISTERCommand_cc_rcsId[] = "$Id: REGISTERCommand.cc,v 1.10 2001/02/20 22:31:04 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool REGISTERCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	}

	static const char* queryHeader = "INSERT INTO channels (name, flags, registered_ts, channel_ts, channel_mode, last_updated) "; 
	static const char* addUserQueryHeader = "INSERT INTO levels (channel_id, user_id, access, flags, added, added_by, last_modif, last_modif_by, last_updated) ";
 
	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false; 

 	/*
	 *  First, check the channel isn't already registered.
	 */

	sqlChannel* theChan; 
	theChan = bot->getChannelRecord(st[1]);
	if (theChan) 
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::chan_already_reg,
				string("%s is already registered with me.")).c_str(), 
			st[1].c_str());
		return false;
	} 

	sqlUser* tmpUser = bot->getUserRecord(st[2]);
	if (!tmpUser) 
		{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::not_registered,
				string("The user %s doesn't appear to be registered.")).c_str(),
			st[2].c_str());
		return true;
		}
 
	/*
	 *  Check the user has sufficient access for this command..
	 */ 

	int level = bot->getAdminAccessLevel(theUser);
	if (level < level::registercmd)
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::insuf_access,
				string("You have insufficient access to perform that command.")));
		return false;
	} 

	string::size_type pos = st[1].find_first_of( ',' ); /* Don't allow comma's in channel names. :) */
 
	if ( (st[1][0] != '#') || (string::npos != pos))
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::inval_chan_name,
				string("Invalid channel name.")));
		return false;
	}
 
	/* If the channel exists, grab the creation timestamp and use this as the channel_ts in the Db. */
	unsigned int channel_ts = 0;
	Channel* tmpChan = Network->findChannel(st[1]);
	channel_ts = tmpChan ? tmpChan->getCreationTime() : ::time(NULL);
 
	/*
	 *  Now, build up the SQL query & execute it!
	 */ 
	strstream theQuery; 

	ExecStatusType status; 

	theQuery << queryHeader << "VALUES (" 
	<< "'" << escapeSQLChars(st[1]) << "',"
	<< "0" << ","
	<< ::time(NULL) << ","
	<< channel_ts << ","
	<< "'+tn'" << ","
	<< ::time(NULL)
	<< ");"
	<< ends; 
 
	elog << "sqlQuery> " << theQuery.str() << endl; 

	if ((status = bot->SQLDb->Exec(theQuery.str())) == PGRES_COMMAND_OK)
	{
		bot->logAdminMessage("%s has registered %s", theUser->getUserName().c_str(), st[1].c_str());
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::regged_chan,
				string("Registered channel %s")).c_str(), 
			st[1].c_str());
	} else {
		bot->Notice(theClient, "Unable to commit channel record, channel may be purged.");
 	}

 	delete[] theQuery.str();

	/*
	 *  Now add the chap at 500 in the new channel.
	 */
 
	sqlChannel* tmpSqlChan = bot->getChannelRecord(st[1]);
	if (!tmpSqlChan) return false;

	strstream addUserQuery;

	addUserQuery << addUserQueryHeader << "VALUES (" 
	<< tmpSqlChan->getID() << ","
	<< tmpUser->getID() << ","
	<< "500,0," 
	<< bot->currentTime() << ","
	<< "'" << theClient->getNickUserHost() << "',"
	<< bot->currentTime() << ","
	<< "'" << theClient->getNickUserHost() << "'," 
	<< bot->currentTime()
	<< ");"
	<< ends; 
 
	elog << "REGISTER::sqlQuery> " << addUserQuery.str() << endl; 

	status = bot->SQLDb->Exec(addUserQuery.str()) ;
	if( PGRES_COMMAND_OK != status )
	{
		bot->Notice(theClient, "Unable to add level 500 user to channel");
	}
 	
	delete[] addUserQuery.str();
	return true ;
} 

} // namespace gnuworld.
