/* REMIGNORECommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h" 
#include	"responses.h"

const char REMIGNORECommand_cc_rcsId[] = "$Id: REMIGNORECommand.cc,v 1.3 2001/02/16 20:20:26 plexus Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool REMIGNORECommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false; 
 
	int admLevel = bot->getAdminAccessLevel(theUser);
	if (admLevel < level::remignore)
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::insuf_access,
				string("Sorry, you have insufficient access to perform that command.")));
		return false;
	} 
 
	for( cservice::silenceListType::iterator ptr = bot->silenceList.begin() ;
		ptr != bot->silenceList.end() ; ++ptr )
	{
		if ( string_lower(st[1]) == string_lower(ptr->second) )
		{
			strstream s;
			s << bot->getCharYYXXX() << " SILENCE " << bot->getCharYYXXX() << " -" << ptr->second << ends; 
			bot->Write( s );
			delete[] s.str(); 

			bot->silenceList.erase(ptr);
			bot->Notice(theClient, 
				bot->getResponse(theUser,
					language::unsilenced,
					string("Removed %s from my silence list")).c_str(),
				st[1].c_str()); 
			return true;
		}

	}

 	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::couldnt_find_silence,
			string("Couldn't find %s in my silence list")).c_str(),
		st[1].c_str()); 
	return true ;
} 

} // namespace gnuworld.
