/* VERIFYCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
 
const char VERIFYCommand_cc_rcsId[] = "$Id: VERIFYCommand.cc,v 1.2 2000/12/26 03:33:35 gte Exp $" ;

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
		bot->Notice(theClient, "%s is a well-known TROUBLEMAKER", target->getNickName().c_str());
		return false;
	}
	if (level == 0) 
	{ 
		bot->Notice(theClient, "%s is NOT an authenticated Cservice representative", target->getNickName().c_str());
		return false;
	}

 	if ((level >= 1) && (level <= 500)) 
	{
		bot->Notice(theClient, "%s is an Official Cservice helper%s", target->getNickName().c_str(), extra.c_str());
		return true;
	}
 
	if ((level >= 501) && (level <= 899)) 
	{
		bot->Notice(theClient, "%s is an Official Cservice admin%s", target->getNickName().c_str(), extra.c_str());
		return true;
	}

	if (level == 900) 
	{
		bot->Notice(theClient, "%s is the Cservice HBIC%s", target->getNickName().c_str(), extra.c_str());
		return true;
	}

	if ((level >= 901) && (level <= 999)) 
	{
		bot->Notice(theClient, "%s is an Official Cservice coder%s", target->getNickName().c_str(), extra.c_str());
		return true;
	}

	if (level == 1000) 
	{
		bot->Notice(theClient, "%s is my daddy%s", target->getNickName().c_str(), extra.c_str());
		return true;
	}
 
	return true ;
} 

} // namespace gnuworld.
