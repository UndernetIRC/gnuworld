/* VERIFYCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"
 
const char VERIFYCommand_cc_rcsId[] = "$Id: VERIFYCommand.cc,v 1.4 2000/12/28 01:21:42 gte Exp $" ;

namespace gnuworld
{
 
using namespace gnuworld;
 
bool VERIFYCommand::Exec( iClient* theClient, const string& Message )
{
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	} 
 
	sqlChannel* theChan = bot->getChannelRecord("*"); // Make sure this channel exists :)

 	string extra;
	extra = "";

	iClient* target = Network->findNick(st[1]); 
	if(!target) {
		bot->Notice(theClient, "Sorry, I don't see %s anywhere.", st[1].c_str());
		return false;
	}

	sqlUser* theUser = bot->isAuthed(target, false);

	/* 
	 *  Firstly, deal with unathenticated users.
	 */

	if (!theUser) 
	{
		if(target->isOper())
		{ 
			bot->Notice(theClient, "%s is an IRC operator", target->getNickName().c_str());
		} else {
			bot->Notice(theClient, "%s is NOT an authenticated Cservice representative", target->getNickName().c_str());
		}
		return false;
	}

 	// TODO: Move all the levels to constants in levels.h

	int level = bot->getAccessLevel(theUser, theChan);
	if (target->isOper()) extra = " and an IRC operator";

	if (level < 0)
	{
		bot->Notice(theClient, "%s is a well-known TROUBLEMAKER (Authenticated as %s)", target->getNickName().c_str(), theUser->getUserName().c_str());
		return false;
	}

	if (level == 0) 
	{ 
		bot->Notice(theClient, "%s is NOT an authenticated Cservice representative (Authenticated as %s)", target->getNickName().c_str(), theUser->getUserName().c_str());
		return false;
	}

 	if ((level >= level::admin::base) && (level <= level::admin::helper)) 
	{
		bot->Notice(theClient, "%s is an Official Cservice Helper%s (Authenticated as %s)", target->getNickName().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}
 
	if ((level > level::admin::helper) && (level <= level::admin::admin)) 
	{
		bot->Notice(theClient, "%s is an Official Cservice Admin%s (Authenticated as %s)", target->getNickName().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}

	if (level == level::admin::hbic) 
	{
		bot->Notice(theClient, "%s is the Cservice HBIC%s (Authenticated as %s)", target->getNickName().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}

	if ((level > level::admin::hbic) && (level <= level::admin::coder)) 
	{
		bot->Notice(theClient, "%s is an Official Cservice Coder%s (Authenticated as %s)", target->getNickName().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}

	if (level == level::admin::daddy) 
	{
		bot->Notice(theClient, "%s is my daddy%s (Authenticated as %s)", target->getNickName().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}
 
	return true ;
} 

} // namespace gnuworld.
