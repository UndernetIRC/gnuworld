/**
 * JUPECommand.cc
 * Jupitur a  server
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
 * $Id: JUPECommand.cc,v 1.23 2006/09/26 17:35:58 kewlio Exp $
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

RCSTAG( "$Id: JUPECommand.cc,v 1.23 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool JUPECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
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
bot->MsgChanLog("JUPE %s\n",st.assemble(1).c_str());

iServer* Server;
string SName;
unsigned int contime;
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
if(Server)
	{
	bot->Notice(theClient,"%s is currently linked to the network"
		    ", it will be automatically squit.",
		    st[1].c_str());
	}
SName = st[1];
contime = ::time(0);

if(!strcasecmp(SName,Network->findServer(bot->getUplink()->getUplinkCharYY())->getName()))
	{
	bot->Notice(theClient,"What are you trying to do? kill me?");
	bot->MsgChanLog("%s just tried to jupe my uplink!\n",theClient->getNickName().c_str());
	return false;
	}
bot->MsgChanLog("%s is asking me to jupe %s because: %s\n",
	theClient->getNickName().c_str(),
	SName.c_str(),
	st.assemble(2).c_str());

string yyxxx( "00]]]" ) ;
iServer* jupeServer = new (std::nothrow) iServer(
	bot->getUplinkIntYY(), // uplinkIntYY
	yyxxx,
	SName,
	time( 0 ),
	st.assemble( 2 ) ) ;
assert( jupeServer != 0 ) ;
jupeServer->setJupe();

// Attach the new (fake) server.
server->AttachServer( jupeServer, bot ) ;

return true ;

}

}
} // namespace gnuworld
