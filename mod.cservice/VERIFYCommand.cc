/* VERIFYCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

 
const char VERIFYCommand_cc_rcsId[] = "$Id: VERIFYCommand.cc,v 1.14 2001/02/28 22:50:36 plexus Exp $" ;

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

	sqlUser* tmpUser = bot->isAuthed(theClient, false);
	iClient* target = Network->findNick(st[1]); 
	if(!target) {
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::dont_see_them,
				string("Sorry, I don't see %s anywhere.")).c_str(), st[1].c_str());
		return false;
	}
        
        if (target->getMode(iClient::MODE_SERVICES)) {
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::is_service_bot,
				string("%s is an Official Undernet Service Bot.")).c_str(), 
			target->getNickName().c_str());
                return false;
	}


	/* 
	 *  Firstly, deal with unauthenticated users.
	 */

	sqlUser* theUser = bot->isAuthed(target, false);

	if (target->isOper()) extra = " and an IRC operator";

	if (!theUser) 
	{
		if(target->isOper())
		{ 
			bot->Notice(theClient, 
				bot->getResponse(tmpUser,
					language::is_an_ircop,
					string("%s is an IRC operator")).c_str(), 
				target->getNickUserHost().c_str());
		} else {
			bot->Notice(theClient, 
				bot->getResponse(tmpUser,
				language::is_not_logged_in,
				string("%s is NOT logged in.")).c_str(), 
			target->getNickUserHost().c_str());
		}
		return false;
	}

        sqlChannel* theChan = bot->getChannelRecord("#coder-com");
	if (!theChan) return true;

 	// TODO: Move all the levels to constants in levels.h

	int level = bot->getAdminAccessLevel(theUser); 
	
	
	int cLevel = bot->getEffectiveAccessLevel(theUser, theChan, false);

	if (level == 0 && cLevel == 0) 
	{ 
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::logged_in_as,
				string("%s is logged in as %s%s")).c_str(), 
			target->getNickUserHost().c_str(), theUser->getUserName().c_str(), extra.c_str());
		return false;
	}

 	if ((level >= level::admin::base) && (level <= level::admin::helper)) 
	{
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::is_cservice_rep,
				string("%s is an Official CService Representative%s and logged in as %s")).c_str(), 
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}
 
	if ((level > level::admin::helper) && (level <= level::admin::admin)) 
	{
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::is_cservice_admin,
				string("%s is an Official CService Administrator%s and logged in as %s")).c_str(), 
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}
 
	if ((level > level::admin::admin) && (level <= level::admin::coder))
	{
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::is_cservice_dev,
				string("%s is an Official CService Developer%s and logged in as %s")).c_str(),
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}

	if ((cLevel >= level::coder::base) && (cLevel <= level::coder::contrib))
	{
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::is_coder_rep,
				string("%s is an Official Coder-Com Representative%s and logged in as %s")).c_str(),
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}

	if ((cLevel > level::coder::base) && (cLevel <= level::coder::contrib))
	{
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::is_coder_rep,
				string("%s is an Official Coder-Com Contributer%s and logged in as %s")).c_str(),
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}

	if ((cLevel > level::coder::contrib) && (cLevel <= level::coder::devel))
	{
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::is_coder_rep,
				string("%s is an Official Coder-Com Developer%s and logged in as %s")).c_str(),
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}

	if (cLevel > level::coder::devel)
	{
		bot->Notice(theClient, 
			bot->getResponse(tmpUser,
				language::is_coder_senior,
				string("%s is an Official Coder-Com Senior%s and logged in as %s")).c_str(),
			target->getNickUserHost().c_str(), extra.c_str(), theUser->getUserName().c_str());
		return true;
	}
 
	return true ;
} 

} // namespace gnuworld.
