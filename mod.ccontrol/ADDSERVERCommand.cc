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
#include	"Constants.h"

const char ADDSERVERCommand_cc_rcsId[] = "$Id: ADDSERVERCommand.cc,v 1.8 2001/12/28 16:28:47 mrbean_ Exp $";

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

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if(st[1].size() > server::MaxName)
	{
	bot->Notice(theClient,"Server name can't be more than %d chars",server::MaxName);
	return false;
	}
string SName;
if(string::npos != st[1].find_first_of('*'))
	{
	iServer* tServer =  Network->findExpandedServerName(st[1]);

	if(!tServer)
		{
		bot->Notice(theClient,"I cant find a linked server that matches %s"
			    ,st[1].c_str());
		return false;
		}
	else
		{
		SName = tServer->getName();
		}
	}
else
	{
	SName = st[1];
	}
ccUser* tmpClient = bot->IsAuth(theClient);
if(tmpClient)
	bot->MsgChanLog("(%s) - %s : ADDSERVER %s\n",tmpClient->getUserName().c_str()
		,theClient->getNickUserHost().c_str(),SName.c_str());
		
ccServer* NewServer = new ccServer(bot->SQLDb);
if(NewServer->loadData(bot->removeSqlChars(SName)))
	{
	bot->Notice(theClient, "Server %s is already in my database!",
		SName.c_str());
	delete NewServer;
	return false;
	}
NewServer->setName(bot->removeSqlChars(SName));
//We need to check if the server is currently connected , 
//if so update all the data
iServer* CurServer = Network->findServerName(SName);
if(CurServer)
	{
	NewServer->setLastNumeric(CurServer->getCharYY());
	NewServer->setLastConnected(CurServer->getConnectTime());
	NewServer->setUplink((Network->findServer(CurServer->getIntYY()))->getName());
	}
NewServer->setAddedOn(::time(0));
NewServer->setLastUpdated(::time(0));
if(NewServer->Insert())
	bot->Notice(theClient,"Server %s added successfully\n",SName.c_str());
else
	bot->Notice(theClient,"Database error while adding server %s\n",SName.c_str());
delete NewServer;
return true;
}
}
} // namespace gnuworld
