/* SHOWIGNORECommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 

const char SHOWIGNORECommand_cc_rcsId[] = "$Id: SHOWIGNORECommand.cc,v 1.5 2001/02/10 01:25:25 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool SHOWIGNORECommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 1 )
	{
		Usage(theClient);
		return true;
	}

	int count = 0;

	if (bot->silenceList.size() != 0) bot->Notice(theClient, "Ignore list:");

	for( cservice::silenceListType::const_iterator ptr = bot->silenceList.begin() ;
		ptr != bot->silenceList.end() ; ++ptr )
	{
		bot->Notice(theClient, "%s", ptr->second.c_str());
		count++;
	}

	if (!count)
		{ 
		bot->Notice(theClient, "Ignore list is empty");
		} else 
		{
		bot->Notice(theClient, "-- End of Ignore List");
		}
	return true ;
} 

} // namespace gnuworld.
