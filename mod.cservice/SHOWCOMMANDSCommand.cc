/* SHOWCOMMANDSCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char SHOWCOMMANDSCommand_cc_rcsId[] = "$Id: SHOWCOMMANDSCommand.cc,v 1.2 2000/12/30 04:15:43 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool SHOWCOMMANDSCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) {
		return false;
	}

	/* 
	 *  Check the channel is actually registered.
	 */

	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
			st[1].c_str());
		return false;
	} 

	/*
 	 *  Fetch the users access level.
	 */

	int level = bot->getAccessLevel(theUser, theChan);

	if (level >= 500) bot->Notice(theClient, "\002Level  500\002: set"); 
	if (level >= 450) bot->Notice(theClient, "\002Level  450\002: addchan join part remchan"); 
	if (level >= 400) bot->Notice(theClient, "\002Level  400\002: adduser clearmode modinfo remuser"); 
	if (level >= 100) bot->Notice(theClient, "\002Level  100\002: deop invite op suspend unsuspend"); 
	if (level >= 75) bot->Notice(theClient,  "\002Level   75\002: ban unban"); 
	if (level >= 50) bot->Notice(theClient,  "\002Level   50\002: kick topic"); 
	if (level >= 1) bot->Notice(theClient,   "\002Level    1\002: status"); 
	if (level >= 0) bot->Notice(theClient,   "\002Level    0\002: access banlist chaninfo deauth help isreg lbanlist login map motd newpass pass showcommands showignore verify"); 
 
	return true ;
} 

} // namespace gnuworld.
