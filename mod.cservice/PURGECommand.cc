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
 * $Id: PURGECommand.cc,v 1.8 2001/03/16 11:50:59 isomer Exp $
 */
 
#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"
#include	"Network.h"
#include	"responses.h"
#include	"cservice_config.h"

const char PURGECommand_cc_rcsId[] = "$Id: PURGECommand.cc,v 1.8 2001/03/16 11:50:59 isomer Exp $" ;

namespace gnuworld
{
 
bool PURGECommand::Exec( iClient* theClient, const string& Message )
{ 
StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	} 

/*
 *  Fetch the sqlUser record attached to this client. If there isn't one,
 *  they aren't logged in - tell them they should be.
 */

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false; 
	}

/*
 *  First, check the channel isn't already registered.
 */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
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
 * Fetch some information about the owner of this channel, so we can
 * 'freeze' it for future investigation in the log.
 */

/* TODO: why is this here? */
strstream managerQuery;
managerQuery	<< "SELECT users.user_name "
		<< "FROM users,levels "
		<< "WHERE levels.user_id = users.id "
		<< "AND levels.access = 500 "
		<< "AND levels.channel_id = "
		<< theChan->getID()
		<< " LIMIT 1"
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlQuery> "
		<< managerQuery.str()
		<< endl; 
#endif


ExecStatusType status = bot->SQLDb->Exec(managerQuery.str()) ;
delete[] managerQuery.str() ;

if( status != PGRES_COMMAND_OK )
	{
	elog	<< "PURGE> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

string manager = bot->SQLDb->GetValue(0,0);

/*
 *  We simply flag this channel as 'deleted', and remove from the cache.
 *  Maintainence scripts can manually remove this channel and all related
 *  artifacts (bans/levels, etc).
 *  It also means accidental deletions won't result in the destruction of a
 *  potentially usefull userlist. :)
 */ 

strstream theQuery ;
theQuery	<< "UPDATE channels set deleted = 1 WHERE id = "
		<< theChan->getID()
		<< ends; 

#ifdef LOG_SQL
	elog	<< "sqlQuery> "
		<< theQuery.str()
		<< endl; 
#endif

status = bot->SQLDb->Exec(theQuery.str()) ;
delete[] theQuery.str() ;

if( status != PGRES_COMMAND_OK )
	{
	elog	<< "PURGE> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

string reason = st.assemble(2);

bot->logAdminMessage("%s (%s) has purged %s (%s)", 
	theClient->getNickName().c_str(),
	theUser->getUserName().c_str(), 
	theChan->getName().c_str(),
	reason.c_str()); 

bot->Notice(theClient, 
	bot->getResponse(theUser, language::purged_chan,
		string("Purged channel %s")).c_str(), 
	st[1].c_str());

bot->writeChannelLog(theChan, 
	theClient, 
	sqlChannel::EV_PURGE,
	theClient->getNickName()+" (" + theUser->getUserName() 
	 + ") has purged " + theChan->getName() + " (" + reason + "), " +
	"Manager was " + manager );

/* Remove from cache.. part channel. */
bot->sqlChannelCache.erase(theChan->getName());
bot->sqlChannelIDCache.erase(theChan->getID());
bot->getUplink()->UnRegisterChannelEvent( theChan->getName(), bot ) ;
bot->Part(theChan->getName());

delete(theChan); 

return true ;
} 

} // namespace gnuworld.
