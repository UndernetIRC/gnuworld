/* REMIGNORECommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h" 
#include	"responses.h"
#include	"Network.h"

const char REMIGNORECommand_cc_rcsId[] = "$Id: REMIGNORECommand.cc,v 1.5 2001/03/14 22:49:14 gte Exp $" ;

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
		if ( string_lower(st[1]) == string_lower(ptr->first.c_str()) )
		{
			strstream s;
			s << bot->getCharYYXXX() << " SILENCE * -" << ptr->first.c_str() << ends; 
			bot->Write( s );
			delete[] s.str(); 

			/* 
			 * Locate this user by numeric.
			 * If the numeric is still in use, clear the ignored flag.
			 * If someone else has inherited this numeric, no prob,
			 * its cleared anyway. 
			 */
	
			iClient* netClient = Network->findClient(ptr->second.second);
			if (netClient)
				{
					bot->setIgnored(netClient, false);
				}
 
			bot->silenceList.erase(ptr);
			bot->Notice(theClient, 
				bot->getResponse(theUser,
					language::unsilenced,
					string("Removed %s from my silence list")).c_str(),
				st[1].c_str()); 
			bot->logAdminMessage("%s (%s) has removed ignore: %s", 
				theClient->getNickName().c_str(), theUser->getUserName().c_str(), st[1].c_str()); 
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
