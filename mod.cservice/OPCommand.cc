/* 
 * OPCommand.cc 
 *
 * 20/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * 28/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Added multilingual support.
 *
 * 01/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Woo! First change of 2001. :)
 * Added duplicate checking to avoid people doing:
 * /msg e op #coder-com Gte Gte Gte Gte Gte Gte Gte {etc}
 * And flooding the target with notices.
 *
 * OP's one or more users on a channel the user as access on.
 *
 * Caveats: None
 *
 * $Id: OPCommand.cc,v 1.17 2001/02/21 00:14:43 dan_karrels Exp $
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

const char OPCommand_cc_rcsId[] = "$Id: OPCommand.cc,v 1.17 2001/02/21 00:14:43 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
 
bool OPCommand::Exec( iClient* theClient, const string& Message )
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
if (level < level::op)
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
 *  If the NOOP flag is set, we aren't allowed to op anyone.
 */

if(theChan->getFlag(sqlChannel::F_NOOP))
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::noop_set,
			string("The NOOP flag is set on %s")).c_str(),
		theChan->getName().c_str());
	return false;
	}
 
/*
 *  Loop over the remaining 'nick' parameters, opping them all.
 */

char delim = 0 ;
string source;
StringTokenizer::size_type counter = 2;

iClient* target = 0 ;

/* Offset of first nick in string. */

typedef map < iClient*, int > duplicateMapType; 
duplicateMapType duplicateMap; 

vector< iClient* > opList; // List of clients to op.

if( st.size() < 3 ) /* No nicks provided, assume we op ourself. :) */
	{
	opList.push_back(theClient);
	source = Message;
	delim = ' '; 
	}
else
	{
	string::size_type pos = st[2].find_first_of( ',' ) ; 

	/* Found a comma? */
	if( string::npos != pos )
		{
		/* We'll do a comma seperated search then. */
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

for( ; counter < st2.size() ; ++counter )
while (counter < st2.size())
	{ 
	target = Network->findNick(st2[counter]);

	if(!target)
		{
		bot->Notice(theClient, bot->getResponse(theUser, language::dont_see_them).c_str(),
			st2[counter].c_str());
		continue ;
		} 

	ChannelUser* tmpChanUser = tmpChan->findUser(target) ;

	// User isn't on the channel?
	if (!tmpChanUser)
		{
		bot->Notice(theClient, bot->getResponse(theUser, language::cant_find_on_chan).c_str(), 
			target->getNickName().c_str(), theChan->getName().c_str()); 
		continue ;
		}

	// User is already opped?
	if(tmpChanUser->getMode(ChannelUser::MODE_O))
		{
		bot->Notice(theClient, bot->getResponse(theUser, language::already_opped).c_str(), 
			target->getNickName().c_str(), theChan->getName().c_str());
		continue ;
		} 

	/*
	 *  If the channel has the STRICTOP flag set, we are only allowed to op people who
	 *  are authorised, and have access in this channel.
	 */

	if(theChan->getFlag(sqlChannel::F_STRICTOP))
		{
		sqlUser* authUser = bot->isAuthed(tmpChanUser->getClient(),
					false);

		/* Not authed, don't allow this op. */
		if (!authUser)
			{ 
			bot->Notice(theClient, 
				bot->getResponse(theUser,
					language::strictop_not_authed,
					string("The STRICTOP flag is set on %s (and %s isn't authenticated)")).c_str(),
				theChan->getName().c_str(), tmpChanUser->getNickName().c_str());
			continue ;
			/* Authed but no access? Tough. :) */
			}
		else if (!(bot->getEffectiveAccessLevel(authUser,theChan, false) >= level::op)) 
			{
			bot->Notice(theClient, 
				bot->getResponse(theUser,
					language::strictop_insuf_access,
					string("The STRICTOP flag is set on %s (and %s has insufficient access)")).c_str(),
				theChan->getName().c_str(), authUser->getUserName().c_str()); 
			continue ;
			} 
		}
 
	// Check for duplicates.
	duplicateMapType::iterator ptr = duplicateMap.find(target);

	if(ptr == duplicateMap.end())
		{ 
		// Not a duplicate.
		opList.push_back(target);
		duplicateMap.insert(duplicateMapType::value_type(target, 0)); 

		// Don't send a notice to the person who issued the command.
		if(target != theClient)
			{ 
			sqlUser* tmpTargetUser = bot->isAuthed(target, false);
			if (tmpTargetUser)
				{
				bot->Notice(target,
					bot->getResponse(tmpTargetUser,
					language::youre_opped_by).c_str(),
					theUser->getUserName().c_str());
				}
			else 
				{
				bot->Notice(target,
					bot->getResponse(theUser,
					language::youre_opped_by).c_str(),
					theUser->getUserName().c_str());
				} 
			} // Don't send to person who issued.
	   	} // Not a duplicate.
	}

// Op them. 
bot->Op(tmpChan, opList);

return true ;
} 

} // namespace gnuworld.

