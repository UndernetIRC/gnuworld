/*
 * REMOVESERVERCommand.cc
 *
 * Delete a server for the bot database
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"

const char REMOVESERVERCommand_cc_rcsId[] = "$Id: REMOVESERVERCommand.cc,v 1.5 2001/07/23 10:28:51 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool REMOVESERVERCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

ccServer* NewServer = new ccServer(bot->SQLDb);
assert(NewServer != NULL);

if(!NewServer->loadData(st [ 1 ])) //Check if the server is already in the database
	{
	bot->Notice(theClient, "Server %s is not  in my database!\n",st [ 1 ].c_str());
	delete NewServer;
	return false;
	}
bot->MsgChanLog("Removing server : %s from the database, at the request of %s\n",
		st[1].c_str(),theClient->getNickName().c_str());

NewServer->setName(st[1]);
if(NewServer->Delete())
	{
	bot->Notice(theClient,"Server \002%s\002 has been successfully removed\n",st[1].c_str());
	delete NewServer;
	return true;
	}
else
	{
	bot->Notice(theClient,"Database error while removing server \002%s\002\n",st[1].c_str());
	delete NewServer;
	return false;
	}
return true;
}

}
}
