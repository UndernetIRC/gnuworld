/*
 * BANCommand.cc
 *
 * 13/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 20/01/2001 - David Henriksen <david@itwebnet.dk>
 * BAN Command written.
 * 23/01/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Updated to new structures, changed quite a bit :)
 *
 * Bans a user on a channel, adds this ban to the internal banlist.
 *
 * Caveats: None.
 *
 * $Id: BANCommand.cc,v 1.3 2001/01/24 01:13:51 gte Exp $
 */

#include        <string>
 
#include        "StringTokenizer.h"
#include        "ELog.h" 
#include        "cservice.h"
#include        "Network.h"
#include        "levels.h"
#include		"misc.h"
#include		"responses.h"
#include		"match.h"

const char BANCommand_cc_rcsId[] = "$Id: BANCommand.cc,v 1.3 2001/01/24 01:13:51 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
using namespace level;
 
bool BANCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	
	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	}
 
	// Is the user authorised?
	
	sqlUser* theUser = bot->isAuthed(theClient, true);
	if(!theUser) return false;

	// Is the channel registered?
	
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if(!theChan)
	{
		bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
		return false;
	} 

	/* Check the bot is in the channel. */
	
	if (!theChan->getInChan()) 
	{
		bot->Notice(theClient, "I'm not in that channel!");
		return false;
	}
 
	int oCount = 0;
	int banTime = 3;
	int banLevel = 75;
	string banReason = "";
	
	if(st.size() >= 6) oCount = 3;
	if(st.size() == 5) oCount = 2;
	if(st.size() == 4) oCount = 1;
	
	switch(oCount)
	{       
		case 1:
	 	{
	 		if(!IsNumeric(st[3])) banReason = st.assemble(3);
			break;
		}	
	
		case 2: 
		{
			if(!IsNumeric(st[3]))
			{
			    banReason = st.assemble(3);
			    break;
			}
	
			if(!IsNumeric(st[4]))
			{
			    banReason = st.assemble(4);
			    banTime = atoi(st[3].c_str());
			}
	
			break;
		}
		
		case 3:
		{
			if(!IsNumeric(st[3]))
			{
		    	banReason = st.assemble(3);
			    break;
			}

			if(!IsNumeric(st[4]))
			{
			    banReason = st.assemble(4);
			    banTime = atoi(st[3].c_str());
			    break;
			}

			banTime = atoi(st[3].c_str());
			banLevel = atoi(st[4].c_str());
			banReason = st.assemble(5);
			break;
		} 
	} 
 
	// Check level.
	
	int level = bot->getAccessLevel(theUser, theChan);
	if(level < level::ban)
	{
		bot->Notice(theClient, "Sorry, you have insufficient access to perform that command.");
		return false;
	}

	if(banLevel < 1 || banLevel > 500 || banLevel > level)
	{
		bot->Notice(theClient, "Invalid banlevel range. Valid range is 1-%i.", level);
		return true;
	}

	if(banTime < 1 || banTime > 336)
	{
		bot->Notice(theClient, "Invalid ban duration. Your ban duration can max be 336 hours.");
		return true;
	}

	int banDuration = banTime * 3600;
	string banTarget = st[2];
	int isNick = 0;
	
	if(!bot->validUserMask(banTarget)) isNick = 1;

	if(isNick == 1)
	{
		iClient* aNick = Network->findNick(banTarget);
		if(!aNick)
		{
			bot->Notice(theClient, "Sorry, I cannot find the specified nick.");
			return true;
		}

		/* Ban and kick this user */
	}
       

	/*
 	 *  Get a list of all bans on this channel, try and match this ban and
	 *  find overlapping bans.
	 */
	
	vector< sqlBan* >* banList = bot->getBanRecords(theChan); 
	vector< sqlBan* >::iterator ptr = banList->begin();
	int removed = 0;

	while (ptr != banList->end())
	{
		sqlBan* theBan = *ptr;
		
		if(string_lower(st[2]) == string_lower(theBan->getBanMask()))
		{
			bot->Notice(theClient, "Specified ban is already in my banlist!");
			return true; 
		}  

		/*
		 * Overlapping ban?
		 */ 

		if(match(banTarget.c_str(), theBan->getBanMask().c_str()) == 0)
		{
			banList->erase(ptr);
			ptr = banList->begin(); 
			//theBan->delete();
			delete(theBan);
			removed++;
		} else {
			++ptr;
		} 
	}
 
	/*
	 *	Check the channel currently exists on the network, if so - we can start kicking
	 *  some ass. 
	 */ 

	Channel* theChannel = Network->findChannel(theChan->getName()); 
	if (!theChannel) 
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_is_empty).c_str(), 
		theChan->getName().c_str());
		return false;
	} 

	vector< iClient* > clientsToKick ;

	for(Channel::userIterator chanUsers = theChannel->userList_begin(); chanUsers != theChannel->userList_end(); ++chanUsers)
	{
		ChannelUser* tmpUser = chanUsers->second; 
		/*
		 *  Iterate over channel members, find a match and boot them..
		 */ 

		if(match(banTarget.c_str(), tmpUser->getClient()->getNickUserHost().c_str()) == 0)
		{ 
			clientsToKick.push_back(tmpUser->getClient());
		}
		
	}

	theChannel->setBan(banTarget);
	/* Set the ban :) */

	strstream s;
	s << bot->getCharYYXXX() << " M " << theChannel->getName() << " +b "
	<< banTarget << ends;

	bot->Write( s );
	delete[] s.str();
 
	/* Kick 'em all out. */
	string finalReason = "(" + theUser->getUserName() + ") " + banReason; 
	if( !clientsToKick.empty() )
	{
		bot->Kick( theChannel, clientsToKick, finalReason ) ;
	} 

	/*
	 *  Fill out new ban details.
	 */
	
	sqlBan* newBan = new sqlBan(bot->SQLDb);
	newBan->setBanMask(banTarget);
	newBan->setSetBy(theUser->getUserName());
	newBan->setLevel(banLevel);
	newBan->setExpires(banDuration+::time(NULL));
	newBan->setReason(banReason); 
	
	/* Insert to our internal List. */ 
	banList->push_back(newBan); 
	
	/* Insert this new record into the database. */
	//newBan->insert();

	return true ;
}

} // Namespace GNUWorld.
