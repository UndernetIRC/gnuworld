/*
 * ADDSERVERCommand.cc
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

const char ADDSERVERCommand_cc_rcsId[] = "$Id: ADDSERVERCommand.cc,v 1.3 2001/11/08 23:13:29 mrbean_ Exp $";

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

if(st[1].size() > 128)
	{
	bot->Notice(theClient,"Server name can't be more than 128 chars");
	return false;
	}
ccServer* NewServer = new ccServer(bot->SQLDb);
if(NewServer->loadData(bot->removeSqlChars(st [ 1 ])))
	{
	bot->Notice(theClient, "Server %s is already in my database!",
		st [ 1 ].c_str());
	delete NewServer;
	return false;
	}
NewServer->setName(bot->removeSqlChars(st[1]));
//We need to check if the server is currently connected , 
//if so update all the data
iServer* CurServer = Network->findServerName(st[1]);
if(CurServer)
	{
	NewServer->setLastNumeric(CurServer->getCharYY());
	NewServer->setLastConnected(CurServer->getConnectTime());
	NewServer->setUplink((Network->findServer(CurServer->getIntYY()))->getName());
	}
if(NewServer->Insert())
	bot->Notice(theClient,"Server %s added successfully\n",st[1].c_str());
else
	bot->Notice(theClient,"Database error while adding server %s\n",st[1].c_str());

return true;
}
}
} // namespace gnuworld
