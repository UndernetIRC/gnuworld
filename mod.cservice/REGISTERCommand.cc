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
 * $Id: REGISTERCommand.cc,v 1.9 2001/02/16 20:20:26 plexus Exp $
 */
 
#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"
#include	"Network.h"
#include	"responses.h"

const char REGISTERCommand_cc_rcsId[] = "$Id: REGISTERCommand.cc,v 1.9 2001/02/16 20:20:26 plexus Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool REGISTERCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	static const char* queryHeader = "INSERT INTO channels (name, flags, registered_ts, channel_ts, channel_mode, last_updated) "; 

	strstream theQuery; 
	ExecStatusType status; 
 
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
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::its_bad_mmkay,
				string("Something went wrong: %s")).c_str(), 
			bot->SQLDb->ErrorMessage()); // Log to msgchan here?
 	}
 
	return true ;
} 

} // namespace gnuworld.
