/* ISREGCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
 
const char ISREGCommand_cc_rcsId[] = "$Id: ISREGCommand.cc,v 1.4 2000/12/23 20:03:57 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool ISREGCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}
 
	sqlChannel* theChan = bot->getChannelRecord(st[1]);

	if (theChan) {
		bot->Notice(theClient, "%s is registered.", theChan->getName().c_str());
	} else {
		bot->Notice(theClient, "%s is not registered.", st[1].c_str());
	}

	return true ;
} 

} // namespace gnuworld.

