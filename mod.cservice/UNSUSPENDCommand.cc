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
 * $Id: UNSUSPENDCommand.cc,v 1.7 2001/02/20 23:01:56 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char UNSUSPENDCommand_cc_rcsId[] = "$Id: UNSUSPENDCommand.cc,v 1.7 2001/02/20 23:01:56 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool UNSUSPENDCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
 
	if( st.size() < 3 )
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
			string("Sorry, you are not authorized with me.")));
	    return false;
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

	sqlLevel* usrLevel = bot->getLevelRecord(Target, theChan);
	if(!usrLevel)
	{
	    bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::not_registered,
			string("I don't know who %s is")).c_str(), 
	    	Target->getUserName().c_str(), theChan->getName().c_str()); 
	    return true;
	}

	sqlLevel* aLevel = bot->getLevelRecord(Target, theChan);

	if (aLevel->getSuspendExpire() == 0)
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::isnt_suspended,
				string("%s isn't suspended on %s")).c_str(),
			Target->getUserName().c_str(), theChan->getName().c_str());
		return false;
	}
 
	aLevel->setSuspendExpire(0);
	aLevel->setSuspendBy("");
	aLevel->setLastModif(bot->currentTime());
	aLevel->setLastModifBy( string( "(" + theUser->getUserName() + ") " +theClient->getNickUserHost() ) ); 
	aLevel->commit();

	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::susp_cancelled,
			string("SUSPENSION for %s is cancelled")).c_str(),
		Target->getUserName().c_str());
 
	return true;
} 

} // namespace gnuworld.

