/*
 * REMSERVERCommand.cc
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
#include	"Constants.h"

const char REMSERVERCommand_cc_rcsId[] = "$Id: REMSERVERCommand.cc,v 1.8 2002/03/01 18:27:36 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool REMSERVERCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}
if(st[1].size() > server::MaxName)
	{
	bot->Notice(theClient,"Server name can't be longer than %d chars"
		    ,server::MaxName);	
	return false;
	}

ccServer* tmpServer = bot->getServer(bot->removeSqlChars(st[1]));
if(!tmpServer)
	{
	bot->Notice(theClient, "Server %s is not  in my database!\n",st [ 1 ].c_str());
	return false;
	}	

bot->MsgChanLog("REMSERVER %s\n",st.assemble(1).c_str());

strstream theQuery;  
theQuery        << User::Query
                << " Where lower(server) = '"
 		<< string_lower(tmpServer->getName()) << "'"
		<< ends;


elog << theQuery.str() << endl;
ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;
        
if(PGRES_TUPLES_OK != status)
        {
	bot->Notice(theClient,"Database error, i cant go on with removing the server");	
        return false;
        }

if(bot->SQLDb->Tuples() > 0)
	{
	bot->Notice(theClient,"There are %d users adding to that server, please remove them first"
			,bot->SQLDb->Tuples());
	return false;
	}

bot->MsgChanLog("Removing server : %s from the database, at the request of %s\n",
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
