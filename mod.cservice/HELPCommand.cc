/*
 * HELPCommand.cc
 *
 * 18/02/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version.
 *
 * Outputs channel service help.
 *
 * Caveats: Needs to be written :)
 *
 * $Id: HELPCommand.cc,v 1.3 2001/10/19 00:11:20 gte Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include 	"responses.h"

const char HELPCommand_cc_rcsId[] = "$Id: HELPCommand.cc,v 1.3 2001/10/19 00:11:20 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;

bool HELPCommand::Exec( iClient* theClient, const string& Message )
{
	bot->incStat("COMMANDS.HELP");
	StringTokenizer st( Message ) ;

	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	sqlUser* theUser = bot->isAuthed(theClient, false);
	string msg = bot->getHelpMessage(theUser, string_upper(st[1]));

	if (msg.empty())
	msg = bot->getHelpMessage(theUser, "INDEX");

	if (!msg.empty())
		bot->Notice(theClient, msg);
	else
		bot->Notice(theClient, "There is no help available for that topic.");

	return true ;
}

} // namespace gnuworld.
