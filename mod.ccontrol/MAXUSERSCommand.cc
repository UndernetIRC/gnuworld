/*
 * MAXUSERSCommand.cc 
 *
 * Shows the maximum number of online users ever recorded
 *
 */

#include	<string>
#include	"StringTokenizer.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"

const char MAXUSERSCommand_cc_rcsId[] = "$Id: MAXUSERSCommand.cc,v 1.1 2002/03/01 18:27:36 mrbean_ Exp $";

namespace gnuworld
{

namespace uworld
{

bool MAXUSERSCommand::Exec( iClient* theClient, const string& Message)
{

bot->Notice(theClient,"Maximum number of users ever recorded is : %d",bot->getMaxUsers());
bot->Notice(theClient,"Recorded on %s",bot->convertToAscTime(bot->getDateMax()));
return true;

}

}

} // namespace gnuworld
