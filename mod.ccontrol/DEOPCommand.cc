/*
 * DEOPCommand.cc
 *
 * Cause the bot to deop users on a specific channel
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"Constants.h"
#include	"ccBadChannel.h"

const char DEOPCommand_cc_rcsId[] = "$Id: DEOPCommand.cc,v 1.9 2002/05/25 15:03:57 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool DEOPCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[1].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d chars",channel::MaxName);
	return false;
	}
Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}

ccBadChannel* Chan = bot->isBadChannel(st[1]);
if(Chan)
        {
        bot->Notice(theClient,"Sorry, but you can not change modes in "
                             "this channel because : %s"
                             ,Chan->getReason().c_str());
        return false;
        }
iClient* Target = 0;

typedef map < iClient*, int > duplicateMapType; 
duplicateMapType duplicateMap; 

bot->MsgChanLog("DEOP %s\n",st.assemble(1).c_str());

string mode = "-";
string args = "";

for(unsigned int i=2;i<st.size();i++)
	{
	if(st[i].size() > 64)
	{
		bot->Notice(theClient,"Nick name can't be more than 64 chars");
		return false;
	}
	Target = Network->findNick( st[ i ] ) ;
	if(Target)
		{
		ChannelUser* tmpChanUser = theChan->findUser(Target) ;

		//Check if the user is in the channel and he's not already opped
		if(( tmpChanUser ) && ( tmpChanUser->getMode(ChannelUser::MODE_O) ) && !( Target->getMode(iClient::MODE_SERVICES) ))
			{
			duplicateMapType::iterator ptr = duplicateMap.find(Target);
			if(ptr == duplicateMap.end())
				{ 
				// Not a duplicate.
				duplicateMap.insert(duplicateMapType::value_type(Target, 0)); 
				tmpChanUser->removeMode(ChannelUser::MODE_O);
				mode+='o';
				args += Target->getCharYYXXX() + ' ' ;
				}
			} //if
		} //if
	} // for
if(!args.empty())
	bot->ModeAsServer( theChan, mode + ' ' + args ) ;
return true;
}

}
}			
	
