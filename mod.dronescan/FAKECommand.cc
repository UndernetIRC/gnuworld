/*
 * FAKECommand.cc
 *
 * Allow control of fake clients.
 *
 * 2003-07-27	Jeekay	Initial writing
 */

#include "StringTokenizer.h"

#include "dronescan.h"
#include "dronescanCommands.h"
#include "levels.h"
#include "sqlFakeClient.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ds {

void FAKECommand::Exec( const iClient *theClient, const string& Message, const sqlUser* theUser )
{
	if(theUser->getAccess() < level::fake) return ;

	StringTokenizer st(Message);

	/* Usage:
	 *  FAKE ACTIVATE <number>
	 */

	if(st.size() < 3) {
		Usage(theClient);
		return ;
	}

	string Command = string_upper(st[1]);

	if("ACTIVATE" == Command) {
		string yyxxx( bot->getUplink()->getCharYY() + "]]]" );

		dronescan::fcMapType::const_iterator itr =
			bot->fakeClients.find(atoi(st[2].c_str()));

		if(itr == bot->fakeClients.end()) {
			bot->Reply(theClient, "No client found for id: %u",
				atoi(st[2].c_str())
				);
			return ;
		}

		sqlFakeClient *theFake = itr->second;

/*
		for(dronescan::fcMapType::const_iterator itr =
		    bot->fakeClients.begin() ; itr != bot->fakeClients.end() ;
		    ++itr ) {
			sqlFakeClient *theFake = itr->second;

			bot->Reply(theClient, "Checking %u",
				theFake->getId()
				);

			if(theFake->isActive()) continue;
*/
			bot->Reply(theClient, "Activating %u",
				theFake->getId()
				);

			iClient *fakeClient = new iClient(
				bot->getUplink()->getIntYY(),
				yyxxx,
				theFake->getNickName(),
				theFake->getUserName(),
				"AKAQEK",
				theFake->getHostName(),
				theFake->getHostName(),
				"+i",
				string(),
				0,
				theFake->getRealName(),
				::time(0)
				);

			assert( fakeClient != 0 );

			bot->getUplink()->AttachClient( fakeClient, bot );

			theFake->setFlag(sqlFakeClient::F_ACTIVE);
/*
		}
*/

		return ;
	}

	return ;
}

} // namespace ds

} // namespace gnuworld
