/*
 * SUSPENDCommand.cc
 *
 * 06/01/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version, done with a fever :/
 *
 * Suspends an user on the specified channel, if suspend duration 0
 * is defined, the user will be unsuspended.
 *
 * Caveats: Suspend yourself? No!
 *
 * TODO: /msg x suspend #channel *, suspends all users below your access
 * level.
 *
 * $Id: SUSPENDCommand.cc,v 1.9 2001/02/20 23:01:56 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char SUSPENDCommand_cc_rcsId[] = "$Id: SUSPENDCommand.cc,v 1.9 2001/02/20 23:01:56 gte Exp $" ;

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
 
	// Is the user authorised?
	 
	sqlUser* theUser = bot->isAuthed(theClient, true);
	if(!theUser) return false;

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
	if(level < level::suspend)
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
	    bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::not_registered,
			string("I don't know who %s is")).c_str(),
	    	st[2].c_str());
	    return true;
	}

	int usrLevel = bot->getAccessLevel(Target, theChan);
	if(!usrLevel)
	{
	    bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::doesnt_have_access,
			string("%s doesn't appear to have access in %s.")).c_str(), 
	    	Target->getUserName().c_str(), theChan->getName().c_str());
	    return true; 
    }

	if (level <= usrLevel)
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::suspend_access_higher,
				string("Cannot suspend a user with equal or higher access than your own.")));
		return false;
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
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::bogus_time,
				string("bogus time units")));
		return true;
	    }
	}

	if(finalDuration > 32140800 || finalDuration < 0) // a year.
	{
	    bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::inval_suspend_dur,
			string("Invalid suspend duration.")));
	    return true;
	}

	sqlLevel* aLevel = bot->getLevelRecord(Target, theChan);

	if(finalDuration == 0)
	{
	    bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::susp_cancelled,
			string("SUSPENSION for %s is cancelled")).c_str(),
		Target->getUserName().c_str());

	    aLevel->setSuspendExpire(finalDuration);
	    aLevel->setSuspendBy("");
	    aLevel->commit();
	    return true;
	}
 
	if (aLevel->getSuspendExpire() != 0)
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::already_susp,
				string("%s is already suspended on %s")).c_str(),
			Target->getUserName().c_str(), theChan->getName().c_str());
		return false;
	} 

	aLevel->setSuspendExpire(finalDuration + bot->currentTime());
	aLevel->setSuspendBy(theClient->getNickUserHost());
	aLevel->setLastModif(bot->currentTime());
	aLevel->setLastModifBy( string( "(" + theUser->getUserName() + ") " +theClient->getNickUserHost() ) ); 
	aLevel->commit();
	
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::susp_set,
			string("SUSPENSION for %s will expire in %s")).c_str(),
		Target->getUserName().c_str(),
		bot->prettyDuration(bot->currentTime() - finalDuration ).c_str());
	return true ;
} 

} // namespace gnuworld.

