/*
 * NOMODECommand.cc
 * 
 * Adds a channel to the bad channels list
 *
 */
#include	<string>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"ccBadChannel.h"

const char NOMODECommand_cc_rcsId[] = "$Id: NOMODECommand.cc,v 1.3 2002/11/20 17:56:17 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool NOMODECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[2].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d chars",channel::MaxName);
	return false;
	}

bot->MsgChanLog("NOMODE %s\n",bot->removeSqlChars(st.assemble(1)).c_str());
	    
if(!strcasecmp(st[1],"ADD"))
	{
	if( st.size() < 4 )
	{
	    Usage( theClient ) ;
		return true ;
	}

	ccBadChannel* NewChannel = bot->isBadChannel(st[2]);
	if(NewChannel)
		{
		bot->Notice(theClient,"There is already a NOMODE entry for channel %s",st[2].c_str());
		return true;
		}
	
	NewChannel = new (std::nothrow) ccBadChannel(bot->removeSqlChars(st[2])
					,bot->removeSqlChars(st.assemble(3))
					,theClient->getRealNickUserHost());
	assert(NewChannel != NULL);
	if(!NewChannel->Insert(bot->SQLDb))
		{
		bot->Notice(theClient,"Error while inserting the NOMODE entry to the database");
		return false;
		}
	bot->addBadChannel(NewChannel);
	}
else if(!strcasecmp(st[1],"REM"))
	{
	ccBadChannel* OldChannel = bot->isBadChannel(st[2]);
	if(!OldChannel)
		{
		bot->Notice(theClient,"There is no NOMODE entry for channel %s",st[2].c_str());
		return true;
		}
	
	bot->remBadChannel(OldChannel);
	if(!OldChannel->Delete(bot->SQLDb))
		{
		bot->Notice(theClient,"Error while removing the NOMODE entry from the database");
		return false;
		}
	delete OldChannel;
	}
else
	{
	bot->Notice(theClient,"NOMODE must get ADD/REM as first parameter");
	return true;
	}

	
return true ;
}

} // namepsace uworld

} // namespace gnuworld
