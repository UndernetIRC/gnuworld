/**
 * REMSERVERCommand.cc
 * Delete a server for the bot database
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
 * $Id: REMSERVERCommand.cc,v 1.14 2007/08/28 16:10:03 dan_karrels Exp $
 */

#include	<string>
#include        <iomanip>
#include	<sstream>
#include	<iostream>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"Constants.h"
#include	"gnuworld_config.h"
#include	"dbHandle.h"

RCSTAG( "$Id: REMSERVERCommand.cc,v 1.14 2007/08/28 16:10:03 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::endl ;
using std::ends ;
using std::stringstream ;
using std::string ;

namespace uworld
{

bool REMSERVERCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;

if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}
if(st[1].size() > server::MaxName)
	{
	bot->Notice(theClient,"Server name can't be longer than %d characters",
		    server::MaxName);
	return false;
	}

ccServer* tmpServer = bot->getServer(bot->removeSqlChars(st[1]));
if(!tmpServer)
	{
	bot->Notice(theClient, "Server %s is not in my database!\n",st [ 1 ].c_str());
	return false;
	}	

bot->MsgChanLog("REMSERVER %s\n",st.assemble(1).c_str());

stringstream theQuery;  
theQuery        << User::Query
                << " Where lower(server) = '"
 		<< string_lower(tmpServer->getName()) << "'"
		<< ends;


elog << theQuery.str().c_str() << endl;

if( !bot->SQLDb->Exec( theQuery.str(), true ) )
//if(PGRES_TUPLES_OK != status)
        {
	bot->Notice(theClient,"Database error, unable to remove the server.");	
        return false;
        }

if(bot->SQLDb->Tuples() > 0)
	{
	bot->Notice(theClient,"There are %d users added to that server, please remove them first",
			bot->SQLDb->Tuples());
	return false;
	}

bot->MsgChanLog("Removing server '%s' from the database at the request of %s\n",
		tmpServer->getName().c_str(),theClient->getNickName().c_str());

//NewServer->setName(SName);
if(tmpServer->Delete())
	{
	bot->Notice(theClient,"Server \002%s\002 has been successfully removed\n",tmpServer->getName().c_str());
	bot->remServer(tmpServer);
	delete tmpServer;
	return true;
	}
else
	{
	bot->Notice(theClient,"Database error while removing server \002%s\002\n",tmpServer->getName().c_str());
	return false;
	}
return true;
}

}
}
