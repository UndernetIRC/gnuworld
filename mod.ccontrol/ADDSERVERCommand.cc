/**
 * ADDSERVERCommand.cc
 * Add a new server for the bot database
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
 * $Id: ADDSERVERCommand.cc,v 1.17 2006/09/26 17:35:58 kewlio Exp $
 */

#include	<string>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"Constants.h"

RCSTAG( "$Id: ADDSERVERCommand.cc,v 1.17 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool ADDSERVERCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[1].size() > server::MaxName)
	{
	bot->Notice(theClient,"Server name can't be more than %d "
		"characters in length.",
		server::MaxName);
	return false;
	}

string SName;
if(string::npos != st[1].find_first_of('*'))
	{
	iServer* tServer =  Network->findExpandedServerName(st[1]);

	if(!tServer)
		{
		bot->Notice(theClient,"I can't find a linked server "
			"that matches %s",
			st[1].c_str());
		return false;
		}

	SName = tServer->getName();
	}
else
	{
	SName = st[1];
	}
bot->MsgChanLog("ADDSERVER %s\n",SName.c_str());
		
ccServer* NewServer = new ccServer(bot->SQLDb);
if(NewServer->loadData(SName))
	{
	bot->Notice(theClient, "Server '%s' is already in my database!",
		SName.c_str());
	delete NewServer;
	return false;
	}
NewServer->setName(SName);
bool Report = true;
if(st.size() > 2) 
	{
	if(!strcasecmp(st[2],"-nr"))
		{
		Report = false;
		}
	}

//We need to check if the server is currently connected , 
//if so update all the data
iServer* CurServer = Network->findServerName(SName);
if(CurServer)
	{
	NewServer->setLastNumeric(CurServer->getCharYY());
	NewServer->setLastConnected(CurServer->getConnectTime());
	NewServer->setUplink(
		(Network->findServer(CurServer->getIntYY()))->getName());
	NewServer->setNetServer(CurServer);
	bot->Write("%s V :%s\n",bot->getCharYYXXX().c_str(),
		CurServer->getCharYY().c_str());
	}
NewServer->setReportMissing(Report);	
if(!Report)
	{
	NewServer->setVersion("Virtual Server V1.0123B3");
	}
NewServer->setAddedOn(::time(0));
NewServer->setLastUpdated(::time(0));
if(NewServer->Insert())
	bot->Notice(theClient,"Server '%s' has been added successfully\n",SName.c_str());
else
	bot->Notice(theClient,"Database error while adding server '%s'\n",
		SName.c_str());

bot->addServer(NewServer);
return true;
}
}
} // namespace gnuworld
