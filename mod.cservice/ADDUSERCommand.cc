/* ADDUSERCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"libpq++.h"

const char ADDUSERCommand_cc_rcsId[] = "$Id: ADDUSERCommand.cc,v 1.1 2000/12/27 03:15:42 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool ADDUSERCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 4 )
	{
		Usage(theClient);
		return true;
	}

	static const char* queryHeader = "INSERT INTO levels (channel_id, user_id, access, flags, added, added_by, last_modif, last_modif_by, last_updated) "; 

	strstream theQuery; 
	ExecStatusType status;
	int targetAccess = atoi(st[3].c_str());

 	/*
	 *  First, check the channel is registered.
	 */
 
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
		return false;
	} 

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* theUser = bot->isAuthed(theClient, true);
	if (!theUser) return false; 
 
	/*
	 *  Check the user has sufficient access on this channel.
	 */

	int level = bot->getAccessLevel(theUser, theChan);
	if (level < level::adduser)
	{
		bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
		return false;
	} 

	/*
	 *  Check we aren't trying to add someone with access higher than ours.
	 */

	if (level <= targetAccess)
	{
		bot->Notice(theClient, "Cannot add a user with equal or higher access than your own.");
		return false;
	}

	if ((targetAccess <= 0) || (targetAccess > 500))
	{
		bot->Notice(theClient, "Invalid access level.");
		return false;
	}

	/*
	 *  Check the person we're trying to add is actually registered.
	 */

	sqlUser* targetUser = bot->getUserRecord(st[2]);
	if (!targetUser)
	{
		bot->Notice(theClient, "Sorry, I don't know who %s is.", st[2].c_str());
		return false; 
	}
 
	/*
	 *  Check this user doesn't already have access on this channel.
	 */

	level = bot->getAccessLevel(targetUser, theChan);
	if (level != 0)
	{
		bot->Notice(theClient, "%s is already added to %s with access level %i.", targetUser->getUserName().c_str(), theChan->getName().c_str(), level);
		return false;
	}
 
	/*
	 *  Now, build up the SQL query & execute it!
	 */

	theQuery << queryHeader << "VALUES (" 
	<< theChan->getID() << ","
	<< targetUser->getID() << ","
	<< targetAccess << ","
	<< "0" << ","
	<< ::time(NULL) << ","
	<< "'" << theClient->getNickUserHost() << "',"
	<< ::time(NULL) << ","
	<< "'" << theClient->getNickUserHost() << "'," 
	<< ::time(NULL)
	<< ");"
	<< ends; 
 
	elog << "sqlQuery> " << theQuery.str() << endl; 

	if ((status = bot->SQLDb->Exec(theQuery.str())) == PGRES_COMMAND_OK)
	{
		bot->Notice(theClient, "Added user %s to %s with access level %i", targetUser->getUserName().c_str(), theChan->getName().c_str(), targetAccess);
	} else {
		bot->Notice(theClient, "Something went wrong: %s", bot->SQLDb->ErrorMessage()); // Log to msgchan here.
 	}

	return true ;
} 

} // namespace gnuworld.
