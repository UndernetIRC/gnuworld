/**
 * CHECKNETCommand.cc
 * Check the network stauts
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: CHECKNETCommand.cc,v 1.20 2009/07/26 23:55:52 hidden1 Exp $
 */

#include	<string>
#include	<sstream>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"ccServer.h"
#include	"ccontrol_generic.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: CHECKNETCommand.cc,v 1.20 2009/07/26 23:55:52 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool CHECKNETCommand::Exec( iClient* theClient, const string& )
{
unsigned int TServers = 0;

bot->Notice(theClient,"Checking network status as at %ld\n",::time(NULL));
bot->MsgChanLog("CHECKNET\n");

ccServer* CurServer = 0;
string Msg;
char tNum[512];
std::stringstream s;
int count = 0;

for (ccontrol::serversConstIterator ptr = bot->serversMap_begin() ; 
	ptr != bot->serversMap_end();++ptr)
	{
	//For each server on the database check if its connetcted
	CurServer = ptr->second;

	//If the server isnt connected 
	if(CurServer->getReportMissing() && !CurServer->getNetServer())
		{
		Msg = "Server \002";
		Msg += CurServer->getName(); 
		Msg +="\002 Is missing, ";
		Msg += "Last split: ";
		if(CurServer->getLastSplitted() != 0)
			{
			sprintf(tNum, "%ld", (long)CurServer->getLastSplitted());
			Msg += "[";
			Msg += tNum;
			Msg += "] ";
			Msg += Ago(CurServer->getLastSplitted());
			Msg += " ago, Split Reason:";
			Msg += CurServer->getSplitReason();
			}
		else
			Msg += " unknown ";
			
		Msg += " Last connected: ";
		
		if(CurServer->getLastConnected() != 0)
			{
			sprintf(tNum, "%ld", (long)CurServer->getLastConnected());
			Msg += "[";
			Msg += tNum;
			Msg += "] ";
			Msg += Ago(CurServer->getLastConnected());
			Msg += " ago";
			}
		else
			Msg += " unknown ";
		bot->Notice(theClient,"%s",Msg.c_str());
		TServers++;
		}
	}

/*
for (ccontrol::serversConstIterator ptr = bot->serversMap_begin() ; 
	ptr != bot->serversMap_end();++ptr)
	{
	//For each server on the database check if its connetcted
	CurServer = ptr->second;

	//If the server isnt connected 
	if (!CurServer->getReportMissing() || CurServer->getNetServer())
		{
		if (s.str().size() > 400)
			{
			bot->Notice(theClient, "Server RPINGS: %s", s.str().c_str());
			s.str("");
			count = 0;
			}
		count++;
		if (count > 1)
			s << ", "; 
		s << CurServer->getName() << " (";
		if (CurServer->getLagTime() > 10000)
			{
			s << "\002" << (int) (CurServer->getLagTime() / 1000) << "seconds\002)";
			}
		else if (CurServer->getLagTime() > 5000)
			{
			s << "\002" << CurServer->getLagTime() << "ms\002)";
			}
		else
			{
			s << CurServer->getLagTime() << "ms)";
			}
		}
	}

bot->Notice(theClient, "Server RPINGS: %s", s.str().c_str());
*/
bot->Notice(theClient,"Finished checking the status of the network.\n");
bot->Notice(theClient,"Found a total of %d missing servers\n",TServers);
return true;

}

}
}
