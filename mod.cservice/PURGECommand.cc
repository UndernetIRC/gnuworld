/* 
 * PURGECommand.cc 
 *
 * 24/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Purges a channel.
 *
 * Caveats: None
 *
 * $Id: PURGECommand.cc,v 1.6 2001/03/03 01:51:55 gte Exp $
 */
 
#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"
#include	"Network.h"
#include	"responses.h"

const char PURGECommand_cc_rcsId[] = "$Id: PURGECommand.cc,v 1.6 2001/03/03 01:51:55 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool PURGECommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	} 

	strstream theQuery; 
	ExecStatusType status; 
	string reason = st.assemble(2);
 
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
	if ((!theChan) || (st[1] == "*")) 
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::chan_not_reg,
				string("%s isn't registered with me")).c_str(), 
			st[1].c_str());
		return false;
	} 

	/*
	 *  Check the user has sufficient access for this command..
	 */ 

	int level = bot->getAdminAccessLevel(theUser);
	if (level < level::purge)
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::insuf_access,
				string("You have insufficient access to perform that command")));
		return false;
	} 
 
 	/*
	 * Fetch some information about the owner of this channel, so we can 'freeze' it for
	 * future investigation in the log.
	 */

	strstream managerQuery;
	managerQuery << "SELECT users_lastseen.last_seen,users.email "
	<< "FROM users,users_lastseen,levels "
	<< "WHERE users.id = users_lastseen.user_id "
	<< "AND levels.user_id = users.id "
	<< "AND levels.access = 500 "
	<< "AND levels.channel_id = " << theChan->getID()
	<< " LIMIT 1" << ends;

	elog << "sqlQuery> " << managerQuery.str() << endl; 

	/*
	 *  We simply flag this channel as 'deleted', and remove from the cache.
	 *  Maintainence scripts can manually remove this channel and all related
	 *  artifacts (bans/levels, etc).
	 *  It also means accidental deletions won't result in the destruction of a
	 *  potentially usefull userlist. :)
	 */ 

	theQuery << "UPDATE channels set deleted = 1 WHERE id = "
	<< theChan->getID() << ends; 
 
	elog << "sqlQuery> " << theQuery.str() << endl; 
 
	if ((status = bot->SQLDb->Exec(theQuery.str())) == PGRES_COMMAND_OK)
	{
		bot->logAdminMessage("%s (%s) has purged %s (%s)", 
			theClient->getNickName().c_str(), theUser->getUserName().c_str(), 
			theChan->getName().c_str(), reason.c_str()); 

		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::purged_chan,
				string("Purged channel %s")).c_str(), 
			st[1].c_str());

		bot->writeChannelLog(theChan, theClient, sqlChannel::EV_JOIN, "");

		/* Remove from cache.. part channel. */
		bot->sqlChannelCache.erase(theChan->getName());
		bot->sqlChannelIDCache.erase(theChan->getID());
		bot->getUplink()->UnRegisterChannelEvent( theChan->getName(), bot ) ;
		bot->Part(theChan->getName());
		delete(theChan); 
	} else {
	 	bot->dbErrorMessage(theClient);
 	}

 	
	return true ;
} 

} // namespace gnuworld.
