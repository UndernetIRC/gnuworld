/*
 * SETCommand.cc
 *
 * 06/01/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version, done with a fever :/
 *
 * Suspends an user on the specified channel, if suspend duration 0
 * is defined, the user will be unsuspended.
 *
 * Caveats: None.
 *
 * $Id: SUSPENDCommand.cc,v 1.1 2001/01/11 01:51:56 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"

const char SUSPENDCommand_cc_rcsId[] = "$Id: SUSPENDCommand.cc,v 1.1 2001/01/11 01:51:56 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool SUSPENDCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
 
	if( st.size() < 4 )
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
	// Is the user authorised?
	 
	sqlUser* theUser = bot->isAuthed(theClient, true);
	if(!theUser) return false;

	// Check level.

	int level = bot->getAccessLevel(theUser, theChan);
	if(level < level::suspend)
	{
	    bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
	    return false;
	}
	// Check whether the user is in the access list.
	sqlUser* Target = bot->getUserRecord(st[2]);
	if(!Target)
	{
	    bot->Notice(theClient, "No such user!"); 
	    return true;
	}
	int usrLevel = bot->getAccessLevel(Target, theChan);
	if(usrLevel < 1)
	{
	    bot->Notice(theClient, "No match!"); 
	    return true; 
    }

	int duration = atoi(st[3].c_str());
	string units;
	int finalDuration = duration;
 
	if( st.size() == 5)
	{
	    units = st[4];
	    if(units == "s") { /* mm'kay */ }
	    else if(units == "m") finalDuration = duration * 60;
	    else if(units == "h") finalDuration = duration * 60 * 60;
	    else if(units == "d") finalDuration = duration * 60 * 60 * 24;
	    else
	    {
		bot->Notice(theClient, "bogus time units");
		return true;
	    }
	}

	if(finalDuration > 32140800 || finalDuration < 0) // a year.
	{
	    bot->Notice(theClient, "Invalid suspend duration.");
	    return true;
	}

	sqlLevel* aLevel = bot->getLevelRecord(Target, theChan);

	if(finalDuration == 0)
	{
	    bot->Notice(theClient, "SUSPENSION for %s is cancelled",
		    Target->getUserName().c_str());

	    aLevel->setSuspendExpire(finalDuration);
	    aLevel->setSuspendBy("");
	    aLevel->commit();
	    return true;
	}
 
	aLevel->setSuspendExpire(finalDuration + ::time(NULL));
	aLevel->setSuspendBy(theClient->getNickUserHost());
	aLevel->commit();
	
	bot->Notice(theClient, "SUSPENSION for %s will expire in %s",
		    Target->getUserName().c_str(),
		    bot->prettyDuration(::time(NULL) - finalDuration ).c_str());
	return true ;
} 

} // namespace gnuworld.

