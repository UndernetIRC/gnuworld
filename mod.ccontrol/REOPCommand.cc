/*
 * REOPCommand.cc 
 *
 * Clears all channel ops, and reops the specified nick
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"ccBadChannel.h"

const char REOPCommand_cc_rcsId[] = "$Id: REOPCommand.cc,v 1.1 2002/08/27 19:22:06 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;
using std::vector ;
using gnuworld::iClient;

namespace uworld
{

bool REOPCommand::Exec( iClient* theClient, const string& Message )
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
	bot->Notice( theClient, "Unable to find channel %s",
		st[ 1 ].c_str() ) ;
	return true ;
	}
if(bot->isOperChan(theChan))
	{
	bot->Notice(theClient,"C'mon , you know you cant reop an oper channel");
	return false;
	}
iClient* reopClient = Network->findNick(st[2]);
if(!reopClient)
	{
	bot->Notice(theClient,"I cant find %s anywere",st[2].c_str());
	return true;
	}
	

bot->MsgChanLog("REOP %s\n",st.assemble(1).c_str());
ccBadChannel* Chan = bot->isBadChannel(st[1]);
if(Chan)
        {
        bot->Notice(theClient,"Sorry, but you can not change modes in "
                             "this channel because : %s"
                             ,Chan->getReason().c_str());
        return false;
        }

string modes = "-";
string args = "";
for( Channel::const_userIterator ptr = theChan->userList_begin();
	ptr != theChan->userList_end() ; ++ptr )
	{
	if( ptr->second->getMode(ChannelUser::MODE_O))
		{
		/* Don't deop +k things */
		if ( !ptr->second->getClient()->getMode(iClient::MODE_SERVICES) ) 
			{
			modes+= 'o';
			args+= ptr->second->getCharYYXXX() + " ";
			ptr->second->removeMode(ChannelUser::MODE_O);
			}
		} // If opped.
	if(modes.size() > 5) //if we got more than 5 , set the mode and continue
		{
		bot->ModeAsServer( theChan, modes + ' ' + args ) ;
		modes = "-";
		args = "";
		}
	}
modes+= "+o";
args += reopClient->getCharYYXXX(); 
	bot->ModeAsServer(theChan,modes + " " + args);
return true;	
}

}
}
