/*
 * DEOPCommand.cc
 *
 * Cause the bot to deop users on a specific channel
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"

const char DEOPCommand_cc_rcsId[] = "$Id: DEOPCommand.cc,v 1.1 2001/03/10 18:33:23 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


bool DEOPCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}

iClient* Target = 0;

typedef map < iClient*, int > duplicateMapType; 
duplicateMapType duplicateMap; 


string mode = "-";
string args = "";

for(unsigned int i=2;i<st.size();i++)
	{
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
	