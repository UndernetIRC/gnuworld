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
 * $Id: PURGECommand.cc,v 1.1 2001/01/24 01:13:52 gte Exp $
 */
 
#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"
#include	"Network.h"

const char PURGECommand_cc_rcsId[] = "$Id: PURGECommand.cc,v 1.1 2001/01/24 01:13:52 gte Exp $" ;

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
	if (!theChan) 
	{
		bot->Notice(theClient, "%s isn't registered with me", st[1].c_str());
		return false;
	} 

	/*
	 *  Check the user has sufficient access for this command..
	 */ 

	int level = bot->getAdminAccessLevel(theUser);
	if (level < level::purge)
	{
		bot->Notice(theClient, "You have insufficient access to perform that command");
		return false;
	} 

 
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

		bot->Notice(theClient, "Purged channel %s", st[1].c_str());

		/* Remove from cache.. part channel. */
		bot->sqlChannelCache.erase(theChan->getName()); 
		bot->getUplink()->UnRegisterChannelEvent( theChan->getName(), bot ) ;
		bot->Part(theChan->getName());
		delete[] theChan;
	} else {
		bot->Notice(theClient, "Something went wrong: %s", bot->SQLDb->ErrorMessage()); // Log to msgchan here?
 	}

 	
	return true ;
} 

} // namespace gnuworld.
