/*
 * UNSUSPENDCommand.cc
 *
 * 06/01/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version.
 *
 * Unsuspends a specified user on a channel.
 *
 * Caveats: None.
 *
 * $Id: UNSUSPENDCommand.cc,v 1.12 2001/07/07 22:51:25 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char UNSUSPENDCommand_cc_rcsId[] = "$Id: UNSUSPENDCommand.cc,v 1.12 2001/07/07 22:51:25 gte Exp $" ;

namespace gnuworld
{

using std::string ;
using namespace level;
 
bool UNSUSPENDCommand::Exec( iClient* theClient, const string& Message )
{ 
StringTokenizer st( Message ) ;
 
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}
 
// Is the user authorized?
	 
sqlUser* theUser = bot->isAuthed(theClient, true);
if(!theUser)
	{
	bot->Notice(theClient, 
	bot->getResponse(theUser,
		language::no_longer_auth,
		string("Sorry, you are not authorised with me.")));
	return false;
	}

/*
 * Trying to unsuspend a user, or a channel?
 * If there is no #, check this person's admin access.
 * If it passes, we can unsuspend a whole user account.
 * (Level 600)
 */

if (st[1][0] != '#')
{
	// Got enough admin access?
	int level = bot->getAdminAccessLevel(theUser);
	if (level < level::globalsuspend)
	{
		Usage(theClient);
		return true;
	} 
 
	// Does this user account even exist?
	sqlUser* targetUser = bot->getUserRecord(st[1]);
	if (!targetUser)
		{
		bot->Notice(theClient, 
			bot->getResponse(theUser, language::not_registered,
				string("I don't know who %s is")).c_str(),
		    	st[1].c_str());
		return true;
		}

	if (!targetUser->getFlag(sqlUser::F_GLOBAL_SUSPEND)) 
	{
		bot->Notice(theClient, "%s isn't suspended.", targetUser->getUserName().c_str());
		return true;
	}

	// Unsuspend them.
	targetUser->removeFlag(sqlUser::F_GLOBAL_SUSPEND);
	targetUser->commit();
	bot->Notice(theClient, "%s has been unsuspended.",
		targetUser->getUserName().c_str());

	targetUser->writeEvent(sqlUser::EV_UNSUSPEND, theUser, "");

	bot->logAdminMessage("%s (%s) has unsuspended %s's user account.",
	theClient->getNickName().c_str(), theUser->getUserName().c_str(),
	targetUser->getUserName().c_str()); 

	return true;
}

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}


// Is the channel registered?

sqlChannel* theChan = bot->getChannelRecord(st[1]);

if(!theChan)
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::chan_not_reg,
			string("Sorry, %s isn't registered with me.")).c_str(), 
		st[1].c_str());
	return false;
	} 

// Check level.
int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if(level < level::unsuspend)
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
	return false;
	}

// Check whether the user is in the access list.
sqlUser* Target = bot->getUserRecord(st[2]);
if(!Target)
	{
	bot->Notice(theClient, "I don't know who %s is",
    	st[2].c_str()); 
	return true;
	}

sqlLevel* aLevel = bot->getLevelRecord(Target, theChan);
if(!aLevel)
	{
	bot->Notice(theClient, 
	bot->getResponse(theUser,
		language::not_registered,
		string("I don't know who %s is")).c_str(), 
    	Target->getUserName().c_str(), theChan->getName().c_str()); 
	return true;
	} 

if (aLevel->getSuspendExpire() == 0)
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::isnt_suspended,
			string("%s isn't suspended on %s")).c_str(),
		Target->getUserName().c_str(), theChan->getName().c_str());
	return false;
	}

/*
 *  Finally, check we have access to perform the unsuspend.
 */

if ((aLevel->getAccess()) >= level)
	{
	bot->Notice(theClient,
		"Cannot unsuspend a user with equal or higher access than your own.");
	return false;
	}

aLevel->setSuspendExpire(0);
aLevel->setSuspendBy(string());
aLevel->setLastModif(bot->currentTime());
aLevel->setLastModifBy( string( "("
	+ theUser->getUserName()
	+ ") "
	+ theClient->getNickUserHost() ) ); 

if( !aLevel->commit() )
	{
	bot->Notice( theClient,
		"Error updating channel status." ) ;
	elog	<< "UNSUSPEND> SQL error"
		<< endl ;
	return false ;
	}

bot->Notice(theClient, 
	bot->getResponse(theUser,
		language::susp_cancelled,
		string("SUSPENSION for %s is cancelled")).c_str(),
	Target->getUserName().c_str());
 
return true;
} 

} // namespace gnuworld.

