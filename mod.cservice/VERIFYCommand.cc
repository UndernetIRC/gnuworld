/* VERIFYCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"

 
const char VERIFYCommand_cc_rcsId[] = "$Id: VERIFYCommand.cc,v 1.9 2001/01/20 22:01:01 gte Exp $" ;

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
 
 	string extra;
	extra = "";

	iClient* target = Network->findNick(st[1]); 
	if(!target) {
		bot->Notice(theClient, "Sorry, I don't see %s anywhere.", st[1].c_str());
		return false;
	}

	sqlUser* theUser = bot->isAuthed(target, false);

	/* 
	 *  Firstly, deal with unauthenticated users.
	 */

	if (target->isOper()) extra = " and an IRC operator";

	if (!theUser) 
	{
		if(target->isOper())
		{ 
			bot->Notice(theClient, "%s is an IRC operator", target->getNickUserHost().c_str());
		} else {
			bot->Notice(theClient, "%s is NOT logged in.", target->getNickUserHost().c_str());
		}
		return false;
	}

 	// TODO: Move all the levels to constants in levels.h

	int level = bot->getAdminAccessLevel(theUser); 
 
	if (level == 0) 
	{ 
		bot->Notice(theClient, "%s is logged in as %s%s", 
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return false;
	}

 	if ((level >= level::admin::base) && (level <= level::admin::helper)) 
	{
		bot->Notice(theClient, "%s is an Official CService Representative%s and logged in as %s", 
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}
 
	if ((level > level::admin::helper) && (level <= level::admin::admin)) 
	{
		bot->Notice(theClient, "%s is an Official CService Administrator%s and logged in as %s", 
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}
 
	if (level == level::admin::coder) 
	{
		bot->Notice(theClient, "%s is an Official Cservice Developer%s and logged in as %s",
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}
 
	return true ;
} 

} // namespace gnuworld.
