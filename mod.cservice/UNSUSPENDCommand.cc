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
 * $Id: UNSUSPENDCommand.cc,v 1.2 2001/01/14 23:12:09 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"

const char UNSUSPENDCommand_cc_rcsId[] = "$Id: UNSUSPENDCommand.cc,v 1.2 2001/01/14 23:12:09 gte Exp $" ;

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
 
	// Is the channel registered?
	
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if(!theChan)
	{
	    bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
	    return false;
	} 
	// Is the user authorized?
	 
	sqlUser* theUser = bot->isAuthed(theClient, true);
	if(!theUser)
	{
	    bot->Notice(theClient, "Sorry, you are not authorized with me.");
	    return false;
	}
	// Check level.

	int level = bot->getAccessLevel(theUser, theChan);
	if(level < level::unsuspend)
	{
	    bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
	    return false;
	}
	// Check whether the user is in the access list.
	sqlUser* Target = bot->getUserRecord(st[2]);
	if(!Target)
	{
	    bot->Notice(theClient, "No such user!");
	    bot->Notice(theClient, "Done.");
	    return true;
	}

	sqlLevel* usrLevel = bot->getLevelRecord(Target, theChan);
	if(!usrLevel)
	{
	    bot->Notice(theClient, "%s doesn't appear to have access in %s.", 
	    	Target->getUserName().c_str(), theChan->getName().c_str()); 
	    return true;
	}

	sqlLevel* aLevel = bot->getLevelRecord(Target, theChan);

        bot->Notice(theClient, "SUSPENSION for %s is cancelled",
		    Target->getUserName().c_str());

        aLevel->setSuspendExpire(0);
        aLevel->setSuspendBy("");
        aLevel->commit();

        return true;
} 

} // namespace gnuworld.

