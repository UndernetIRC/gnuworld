/*
 * CHECKNETCommand.cc
 *
 * Check the network stauts
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"ccServer.h"

const char CHECKNETCommand_cc_rcsId[] = "$Id: CHECKNETCommand.cc,v 1.10 2002/03/01 18:27:36 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool CHECKNETCommand::Exec( iClient* theClient, const string& Message )
{

unsigned int TServers = 0;

bot->Notice(theClient,"Checking network status as for %d\n",::time(NULL));
bot->MsgChanLog("CHECKNET\n");
ccServer* CurServer;
string Msg;
char tNum[512];		    
for (ccontrol::serversConstIterator ptr = bot->serversMap_begin() ; 
	ptr != bot->serversMap_end();++ptr)
	{ //For each server on the database check if its connetcted
	CurServer = ptr->second;
	if(!CurServer->getNetServer()) //If the server isnt connected 
		{
		Msg = "Server \002";
		Msg += CurServer->getName(); 
		Msg +="\002 Is missing, ";
		Msg += "Last split time : ";
		if(CurServer->getLastSplitted() != 0)
			{
			sprintf(tNum,"%li",CurServer->getLastSplitted());
			Msg += tNum;
			Msg += " Split Reason:";
			Msg += CurServer->getSplitReason();
			}
		else
			Msg += " unknown ";
			
		Msg += " Last connection time : ";
		
		if(CurServer->getLastConnected() != 0)
			{
			sprintf(tNum,"%li",CurServer->getLastConnected());
			Msg += tNum;
			}
		else
			Msg += " unknown ";
		bot->Notice(theClient,"%s",Msg.c_str());
		TServers++;
		}
	}


bot->Notice(theClient,"Finished checking the network status\n");
bot->Notice(theClient,"Found a total of %d missing servers\n",TServers);
return true;

}

}
}
