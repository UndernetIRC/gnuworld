/*
 * AI.cc. Artificial Intelligence unit.
 */

#include	<universe.h>
#include	"sqlUser.h"

const char AICommand_cc_rcsId[] = "$Id: AI.cc,v 1.1 2002/01/13 01:30:31 gte Exp $" ;

namespace gnuworld
{

bool AICommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.AI");

/*
 *  a2b57e6118e750f3b8cde7fa8d7c4114
 */

return true ;
}

} // namespace gnuworld.
