/* ISREGCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"responses.h"
 
const char ISREGCommand_cc_rcsId[] = "$Id: ISREGCommand.cc,v 1.5 2001/02/16 20:20:26 plexus Exp $" ;

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
 
	sqlUser* theUser = bot->isAuthed(theClient, false);
	sqlChannel* theChan = bot->getChannelRecord(st[1]);

	if (theChan) {
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::is_reg,
				string("%s is registered.")).c_str(), 
			theChan->getName().c_str());
	} else {
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::is_not_reg,
				string("%s is not registered.")).c_str(), 
			st[1].c_str());
	}

	return true ;
} 

} // namespace gnuworld.

