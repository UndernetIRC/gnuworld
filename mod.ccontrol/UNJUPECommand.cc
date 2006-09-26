/**
 * UNJUPECommand.cc
 * UNJupitur a  server
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
 * $Id: UNJUPECommand.cc,v 1.3 2006/09/26 17:36:01 kewlio Exp $
 */

#include	<new>
#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"iServer.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: UNJUPECommand.cc,v 1.3 2006/09/26 17:36:01 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool UNJUPECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return false ;
	}

// The server name to be juped must have at least 1 '.'
if( st[1].length() > server::MaxName) 
	{
	bot->Notice( theClient, "Bogus server name" ) ;
	return false ;
	}
bot->MsgChanLog("UNJUPE %s\n",st.assemble(1).c_str());

iServer* Server;

if(string::npos != st[ 1 ].find_first_of( '*' ))
	{
	bot->Notice(theClient,"Sorry, but you must give a full server name when juping!");
	return false;
	}	
else if(string::npos == st[ 1 ].find_first_of( '.' ))
	{
	bot->Notice( theClient, "Bogus server name" ) ;
	return false ;
	}

Server = Network->findServerName(st[1]);
if(!Server)
	{
	bot->Notice(theClient,"Sorry, but i can't find that server!");
	return false;
	}
if(!Server->isJupe())
	{
	bot->Notice(theClient,"Sorry, but the server is not juped!");
	return false;
	}
server->DetachServer(Server);


return true ;

}

}
} // namespace gnuworld
