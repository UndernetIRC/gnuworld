/* ISREGCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"sqlChannel.h"

const char ISREGCommand_cc_rcsId[] = "$Id: ISREGCommand.cc,v 1.2 2000/12/11 21:36:09 gte Exp $" ;

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
 
	sqlChannel* theChan = new sqlChannel(bot->SQLDb);

	if (theChan->loadData(st[1])) {
		bot->Notice(theClient, "%s is registered.", theChan->getName().c_str());
	} else {
		bot->Notice(theClient, "%s is not registered.", st[1].c_str());
	}

	return true ;
} 

} // namespace gnuworld.

