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

const char REMSERVERCommand_cc_rcsId[] = "$Id: REMSERVERCommand.cc,v 1.5 2001/12/23 09:07:57 mrbean_ Exp $";

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

string SName = bot->expandDbServer(bot->removeSqlChars(st[1]));
if(!strcasecmp(SName,""))
	{
	bot->Notice(theClient, "Server %s is not  in my database!\n",st [ 1 ].c_str());
	return false;
	}	

ccUser* tmpUser = bot->IsAuth(theClient);
if(tmpUser)
        bot->MsgChanLog("(%s) - %s : REMSERVER %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());

strstream theQuery;  
theQuery        << User::Query
                << " Where lower(server) = '"
 		<< string_lower(SName) << "'"
		<< ends;


elog << theQuery.str() << endl;
ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;
        
if(PGRES_TUPLES_OK != status)
        {
	bot->Notice(theClient,"Got a db error, adding is aborted");	
        return false;
        }

if(bot->SQLDb->Tuples() > 0)
	{
	bot->Notice(theClient,"There are %d users adding to that server, please remove them first"
			,bot->SQLDb->Tuples());
	return false;
	}
ccServer* NewServer = new ccServer(bot->SQLDb);
assert(NewServer != NULL);

if(!NewServer->loadData(SName)) //Check if the server is already in the database
	{
	bot->Notice(theClient, "Server %s is not  in my database!\n",SName.c_str());
	delete NewServer;
	return false;
	}
bot->MsgChanLog("Removing server : %s from the database, at the request of %s\n",
		SName.c_str(),theClient->getNickName().c_str());

//NewServer->setName(SName);
if(NewServer->Delete())
	{
	bot->Notice(theClient,"Server \002%s\002 has been successfully removed\n",SName.c_str());
	delete NewServer;
	return true;
	}
else
	{
	bot->Notice(theClient,"Database error while removing server \002%s\002\n",SName.c_str());
	delete NewServer;
	return false;
	}
return true;
}

}
}
