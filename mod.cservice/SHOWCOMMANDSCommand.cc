/* SHOWCOMMANDSCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char SHOWCOMMANDSCommand_cc_rcsId[] = "$Id: SHOWCOMMANDSCommand.cc,v 1.10 2001/02/10 03:49:10 isomer Exp $" ;

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

	int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
	int admin = bot->getEffectiveAccessLevel(theUser, bot->getChannelRecord("*"), true); 

	if (level >= 1000) bot->Notice(theClient,"\002Level 1000\002: anything");
	if (level >= 900) bot->Notice(theClient, "\002Level  900\002: shutdown");
	if (level >= 800) bot->Notice(theClient, "\002Level  800\002: say servnotice");
	if (level >= 750) bot->Notice(theClient, "\002Level  750\002: purge");
	if (level >= 600) bot->Notice(theClient, "\002Level  600\002: register remignore");
	if (level >= 450) bot->Notice(theClient, "\002Level  450\002: join* part* set%s*",(level>=500) ? "+" : ""); 
	if (level >= 400) bot->Notice(theClient, "\002Level  400\002: adduser clearmode* modinfo remuser"); 
	if (level >= 100) bot->Notice(theClient, "\002Level  100\002: op deop invite suspend unsuspend");
	if (level >= 75) bot->Notice(theClient,  "\002Level   75\002: ban* unban*"); 
	if (level >= 50) bot->Notice(theClient,  "\002Level   50\002: kick%s topic",(level>=200) ? "+" : "");
	if (level == 42) bot->Notice(theClient,  "\002Level   42\002: PANIC");
	if (level >= 25) bot->Notice(theClient,  "\002Level   25\002: voice devoice");
	if (level >= 1) bot->Notice(theClient,   "\002Level    1\002: status%s",(level>=400||admin>0||theClient->isOper()) ? "+" : ""); 
	if (level >= 0) bot->Notice(theClient,   "\002Level    0\002: access banlist chaninfo info help* lbanlist login motd* newpass showcommands showignore verify"); 
	if (admin >= level::force) 
			bot->Notice(theClient,   "\002Level    *\002: force unforce");
	if (theClient->isOper()) 
		           bot->Notice(theClient,"\002Level Oper\002: operjoin* operpart*");
	bot->Notice(theClient, "Commands marked * are not yet fully functional :)");
	return true ;
} 

} // namespace gnuworld.
