/* LOGINCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"responses.h"

const char LOGINCommand_cc_rcsId[] = "$Id: LOGINCommand.cc,v 1.3 2000/12/28 21:19:53 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool LOGINCommand::Exec( iClient* theClient, const string& Message )
{
 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

 	/*
	 *  Check theClient isn't already logged in, if so, tell them they shouldn't be.
	 */
 
	sqlUser* tmpUser = bot->isAuthed(theClient, false);
	if (tmpUser) {
		bot->Notice(theClient, bot->getResponse(tmpUser, language::already_authed).c_str(),
			tmpUser->getUserName().c_str());
		return false;
	}

	/*
	 *  Find the user record, confirm authorisation and attach the record to this client. 
	 */
 
	sqlUser* theUser = bot->getUserRecord(st[1]);
	if (theUser) {
		theClient->setCustomData(bot, (void *)theUser);
		/*
		 *  Compare password with MD5 hash stored in user record.
		 */

		bot->Notice(theClient, bot->getResponse(theUser, language::auth_success).c_str(), 
			theUser->getUserName().c_str()); 
	} else
	{
		bot->Notice(theClient, "Sorry, I don't know who %s is.", st[1].c_str());
		return false;
	}

	return true; 
} 

} // namespace gnuworld.
