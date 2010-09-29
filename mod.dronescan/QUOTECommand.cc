/*
 * QUOTECommand.cc
 *
 * Write a user given string directly to the network.
 *
 * 2003-07-27	Jeekay	Initial writing
 */

#include "StringTokenizer.h"

#include "dronescan.h"
#include "dronescanCommands.h"
#include "levels.h"
#include "sqlUser.h"

namespace gnuworld {

namespace ds {

void QUOTECommand::Exec( const iClient *theClient, const string& Message, const sqlUser* theUser )
{
	if(theUser->getAccess() < level::quote) return ;

	StringTokenizer st(Message);

	if(st.size() < 2) {
		Usage(theClient);
		return ;
	}

	bot->log(INFO, "%s (%s) is quoting: %s",
		theClient->getNickName().c_str(),
		theClient->getAccount().c_str(),
		st.assemble(1).c_str()
		);

	bot->Write(st.assemble(1));

	return ;
}

} // namespace ds

} // namespace gnuworld
