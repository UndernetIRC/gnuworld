/* SHOWIGNORECommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"responses.h"

const char SHOWIGNORECommand_cc_rcsId[] = "$Id: SHOWIGNORECommand.cc,v 1.8 2001/03/05 12:46:50 isomer Exp $" ;

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

	sqlUser* theUser = bot->isAuthed(theClient, false);
	
	if (bot->silenceList.size() != 0) bot->Notice(theClient, 
							bot->getResponse(theUser,
								language::ignore_list_start,
								string("Ignore list:")));

	for( cservice::silenceListType::const_iterator ptr = bot->silenceList.begin() ;
		ptr != bot->silenceList.end() ; ++ptr )
		{
		bot->Notice(theClient, bot->getResponse(theUser,
			language::rpl_ignorelist, "%s for %i minutes"),
			ptr->second.c_str(),
			(((ptr->first - bot->currentTime()) / 60) % 60));
		count++;
		}

	if (!count)
		{ 
		bot->Notice(theClient, bot->getResponse(theUser,
						language::ignore_list_empty,
						string("Ignore list is empty")));
		} else 
		{
		bot->Notice(theClient, bot->getResponse(theUser,
						language::ignore_list_end,
						string("-- End of Ignore List")));
		}
	return true ;
} 

} // namespace gnuworld.
