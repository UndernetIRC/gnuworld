/*
 * ADDNEWSERVERCommand.cc
 *
 * Add a new server for the bot database
 *
 */

#include	<string>
#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"

const char ADDNEWSERVERCommand_cc_rcsId[] = "$Id: ADDNEWSERVERCommand.cc,v 1.3 2001/07/17 16:58:27 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;

bool ADDNEWSERVERCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

ccServer* NewServer = new ccServer(bot->SQLDb);
if(NewServer->loadData(st [ 1 ]))
	{
	bot->Notice(theClient, "Server %s is already in my database!",
		st [ 1 ].c_str());
	delete NewServer;
	return false;
	}
NewServer->set_Name(st[1]);
//We need to check if the server is currently connected , 
//if so update all the data
iServer* CurServer = Network->findServerName(st[1]);
if(CurServer)
	{
	NewServer->set_LastNumeric(CurServer->getCharYY());
	NewServer->set_LastConnected(CurServer->getConnectTime());
	NewServer->set_Uplink((Network->findServer(CurServer->getIntYY()))->getName());
	}
if(NewServer->Insert())
	bot->Notice(theClient,"Server %s added successfully\n",st[1].c_str());
else
	bot->Notice(theClient,"Database error while adding server %s\n",st[1].c_str());

return true;
}

} // namespace gnuworld
