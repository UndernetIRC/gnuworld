/* LOGINCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 

const char LOGINCommand_cc_rcsId[] = "$Id: LOGINCommand.cc,v 1.2 2000/12/23 20:03:57 gte Exp $" ;

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
	 *  Check theClient isn't already logged in.
	 */

	sqlUser* tmpUser = (sqlUser*) theClient->getCustomData(bot);
	if (tmpUser) {
		bot->Notice(theClient, "Sorry, You are already authenticated as %s.", tmpUser->getUserName().c_str());
		return false;
	}

	/*
	 *  Find the user record, confirm authorisation and attach the record to this client. 
	 */
 
	sqlUser* theUser = bot->getUserRecord(st[1]);
	if (theUser) {
		theClient->setCustomData(bot, (void *)theUser);
		bot->Notice(theClient, "AUTHENTICATION SUCCESSFUL as %s", theUser->getUserName().c_str()); 
	} else
	{
		bot->Notice(theClient, "Sorry, I don't know who %s is.", st[1].c_str());
		return false;
	}

	return true; 
} 

} // namespace gnuworld.
