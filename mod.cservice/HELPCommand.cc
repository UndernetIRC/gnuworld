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
 * $Id: HELPCommand.cc,v 1.2 2001/09/05 03:47:56 gte Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include 	"responses.h"

const char HELPCommand_cc_rcsId[] = "$Id: HELPCommand.cc,v 1.2 2001/09/05 03:47:56 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;

bool HELPCommand::Exec( iClient* theClient, const string& Message )
{
	bot->incStat("COMMANDS.HELP");
	StringTokenizer st( Message ) ;
	if( st.size() > 2 )
	{
		Usage(theClient);
		return true;
	}

	bot->Notice(theClient, "There is no help available.");

	return true ;
}

} // namespace gnuworld.
