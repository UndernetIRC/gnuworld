/* 
 * DEOPCommand.cc 
 *
 * 20/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * DEOP's one or more users on a channel the user as access on.
 *
 * Caveats: None
 *
 * $Id: DEOPCommand.cc,v 1.10 2001/03/06 02:34:32 dan_karrels Exp $
 */

#include	<string>
#include	<map>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char DEOPCommand_cc_rcsId[] = "$Id: DEOPCommand.cc,v 1.10 2001/03/06 02:34:32 dan_karrels Exp $" ;

namespace gnuworld
{

using std::map ;

bool DEOPCommand::Exec( iClient* theClient, const string& Message )
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
if (!theUser)
	{
	return false;
	}

/* 
 *  Check the channel is actually registered.
 */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_not_reg).c_str(),
		st[1].c_str());
	return false;
	} 

/* Check the bot is in the channel. */

if (!theChan->getInChan())
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::i_am_not_on_chan,
			string("I'm not in that channel!")));
	return false;
	}

/*
 *  Check the user has sufficient access on this channel.
 */

int level = bot->getEffectiveAccessLevel(theUser, theChan, true);
if (level < level::deop)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::insuf_access).c_str());
	return false;
	} 

Channel* tmpChan = Network->findChannel(theChan->getName()); 
if (!tmpChan) 
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::chan_is_empty).c_str(), 
		theChan->getName().c_str());
	return false;
	}

/*
 * Check we're actually opped first..
 */

ChannelUser* tmpBotUser = tmpChan->findUser(bot->getInstance());
if (!tmpBotUser)
	{
	return false;
	}

if(!tmpBotUser->getMode(ChannelUser::MODE_O))
	{
	bot->Notice(theClient, "I'm not opped in %s", theChan->getName().c_str());
	return false;
	} 

/*
 *  Loop over the remaining 'nick' parameters, opping them all.
 */

char delim = 0 ;
string source;
iClient* target = 0 ;

// Offset of first nick in list.
StringTokenizer::size_type counter = 2 ;

typedef map < iClient*, int > duplicateMapType; 
duplicateMapType duplicateMap; 

vector< iClient* > deopList ;

// No nicks provided, assume we op the requesting client
if( st.size() < 3 )
	{
	deopList.push_back(theClient);
	source = Message;
	delim = ' '; 
	}
else
	{
	string::size_type pos = st[2].find_first_of( ',' ) ; 

	// Found a comma?
	if( string::npos != pos )
		{
		// We'll do a comma seperated search then.
		source = st.assemble(2);
		delim = ',';
		counter = 0;
		}
	else
		{ 
		source = Message;
		delim = ' ';
		} 
	}
 
StringTokenizer st2( source, delim );

while( counter < st2.size())
	{
	target = Network->findNick(st2[counter]);

	if(!target)
		{
		bot->Notice(theClient,
			bot->getResponse(theUser, language::dont_see_them).c_str(),
			st2[counter].c_str());

		++counter ;
		continue ;
		} 

	/* Don't deop +k things */
	if( target->getMode(iClient::MODE_SERVICES) )
		{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::wouldnt_appreciate,
				string("I don't think %s would appreciate that.")).c_str(),
			target->getNickName().c_str());

		++counter ;
		continue ;
		}

	ChannelUser* tmpChanUser = tmpChan->findUser(target) ;

	// User isn't on the channel?
	if( !tmpChanUser )
		{
		bot->Notice(theClient,
			bot->getResponse(theUser, language::cant_find_on_chan).c_str(), 
			target->getNickName().c_str(), theChan->getName().c_str()); 

		++counter ;
		continue ;
		}

	// User isn't opped?
	if( !tmpChanUser->getMode(ChannelUser::MODE_O))
		{
		bot->Notice(theClient,
			bot->getResponse(theUser, language::not_opped).c_str(),
			target->getNickName().c_str(), theChan->getName().c_str());

		++counter ;
		continue ;
		} 
 
	// Check for duplicates.
	duplicateMapType::iterator ptr = duplicateMap.find(target);

	if( ptr != duplicateMap.end() )
		{
		// Duplicate
		++counter ;
		continue ;
		}

	deopList.push_back(target);
	duplicateMap.insert(duplicateMapType::value_type(target, 0)); 

	// Don't send a notice to the person who issued the command.
	if(target != theClient)
		{ 
		sqlUser* tmpTargetUser = bot->isAuthed(target, false);
		if (tmpTargetUser)
			{
			bot->Notice(target,
				bot->getResponse(tmpTargetUser, language::youre_deopped_by).c_str(),
				theClient->getNickName().c_str(),
				theUser->getUserName().c_str());
			}
		else
			{
			bot->Notice(target,
				bot->getResponse(theUser, language::youre_deopped_by).c_str(),
				theClient->getNickName().c_str(),
				theUser->getUserName().c_str());
			} 
		} // Don't send to person who issued.

	++counter ;
	} // while()

// deOp them. 
bot->DeOp(tmpChan, deopList);
return true ;

} 

} // namespace gnuworld.

