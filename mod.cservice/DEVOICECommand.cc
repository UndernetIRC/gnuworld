/* 
 * DEVOICECommand.cc 
 * 
 * 28/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version. 
 *
 * Devoice's one or more users on a channel the user as access on.
 *
 * Caveats: None
 *
 * $Id: DEVOICECommand.cc,v 1.4 2001/01/16 01:31:40 gte Exp $
 */

#include	<string>
#include	<map>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

using std::map ;

const char DEVOICECommand_cc_rcsId[] = "$Id: DEVOICECommand.cc,v 1.4 2001/01/16 01:31:40 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool DEVOICECommand::Exec( iClient* theClient, const string& Message )
{ 
	vector< iClient* > devoiceList; // List of clients to devoice.
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
	if (!theUser) {
		return false;
	}

	/* 
	 *  Check the channel is actually registered.
	 */

	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
			st[1].c_str());
		return false;
	} 

	/*
	 *  Check the user has sufficient access on this channel.
	 */

	int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
	if (level < level::devoice)
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str());
		return false;
	} 

	Channel* tmpChan = Network->findChannel(theChan->getName()); 
	if (!tmpChan) 
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_is_empty).c_str(), 
			theChan->getName().c_str());
		return false;
	}
 

	if( st.size() < 3 ) /* No nicks provided, assume we devoice ourself. :) */
	{
		devoiceList.push_back(theClient);
	}

	/*
	 *  Loop over the remaining 'nick' parameters, opping them all.
	 */

	iClient* target;
	unsigned short counter = 2; // Offset of first nick in list.
	unsigned short cont = true;
	typedef map < iClient*, int > duplicateMapType; 
	duplicateMapType duplicateMap; 
	string source;
	char delim;

	if( st.size() < 3 ) // No nicks provided, assume we op ourself. :)
	{
		devoiceList.push_back(theClient);
		source = Message;
		delim = ' '; 
	} else
	{
		string::size_type pos = st[2].find_first_of( ',' ) ; 
		if( string::npos != pos ) // Found a comma?
		{
			source = st.assemble(2); // We'll do a comma seperated search then.
			delim = ',';
			counter = 0;
		} else { 
			source = Message;
			delim = ' ';
		} 
	}
 
	StringTokenizer st2( source, delim ); 

	while (counter < st2.size())
	{ 
		target = Network->findNick(st2[counter]);

		if(!target)
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::dont_see_them).c_str(),
				st2[counter].c_str());
			cont = false;
		} 

		ChannelUser* tmpChanUser;
		if (cont) tmpChanUser = tmpChan->findUser(target) ;
		if (cont && !tmpChanUser) // User isn't on the channel?
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::cant_find_on_chan).c_str(), 
				target->getNickName().c_str(), theChan->getName().c_str()); 
			cont = false;
		}

		if(cont && !tmpChanUser->getMode(ChannelUser::MODE_V)) // User isn't voiced?
		{
			bot->Notice(theClient, bot->getResponse(theUser, language::not_voiced).c_str(), 
				target->getNickName().c_str(), theChan->getName().c_str());
				cont = false;
		} 
 
	 	if (cont) 
	 	{
			duplicateMapType::iterator ptr = duplicateMap.find(target); // Check for duplicates.
			if(ptr == duplicateMap.end()) // Not a duplicate.
			{ 
				devoiceList.push_back(target);
				duplicateMap.insert(duplicateMapType::value_type(target, 0)); 

				if(target != theClient) // Don't send a notice to the person who issued the command.
				{ 
					sqlUser* tmpTargetUser = bot->isAuthed(target, false);
					if (tmpTargetUser)
					{
						bot->Notice(target, bot->getResponse(tmpTargetUser, language::youre_devoiced_by).c_str(),
							theUser->getUserName().c_str());
					} else 
					{
						bot->Notice(target, bot->getResponse(theUser, language::youre_devoiced_by).c_str(),
							theUser->getUserName().c_str());
					} 
				} // Don't send to person who issued.
		   	} // Not a duplicate.
		}

		cont = true;
		counter++;
	}

	// devoice them. 
	bot->DeVoice(tmpChan, devoiceList);
	return true ;
} 

} // namespace gnuworld.

