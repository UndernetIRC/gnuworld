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
 * $Id: REGISTERCommand.cc,v 1.6 2001/01/17 22:12:15 gte Exp $
 */
 
#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"
#include	"Network.h"

const char REGISTERCommand_cc_rcsId[] = "$Id: REGISTERCommand.cc,v 1.6 2001/01/17 22:12:15 gte Exp $" ;

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
		bot->Notice(theClient, "%s is already registered with me.", st[1].c_str());
		return false;
	} 

	/*
	 *  Check the user has sufficient access for this command..
	 */ 

	int level = bot->getAdminAccessLevel(theUser);
	if (level < level::registercmd)
	{
		bot->Notice(theClient, "You have insufficient access to perform that command.");
		return false;
	} 
 
	if (st[1][0] != '#')
	{
		bot->Notice(theClient, "Invalid channel name.");
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
	<< "'" << st[1] << "',"
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
		bot->Notice(theClient, "Registered channel %s", st[1].c_str());
	} else {
		bot->Notice(theClient, "Something went wrong: %s", bot->SQLDb->ErrorMessage()); // Log to msgchan here?
 	}
 
	return true ;
} 

} // namespace gnuworld.
