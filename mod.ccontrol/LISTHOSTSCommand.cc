/*
 * LISTHOSTSCommand.cc
 *
 * Get an oper hosts entry
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char LISTHOSTSCommand_cc_rcsId[] = "$Id: LISTHOSTSCommand.cc,v 1.1 2001/03/10 18:33:23 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;



bool LISTHOSTSCommand::Exec( iClient* theClient, const string& Message)
{	 
StringTokenizer st( Message ) ;
	
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}
//Fetch the oper data base entry
ccUser *tmpUser = bot->GetUser(st[1]);
if(!tmpUser)
	{
        bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
        return false;
	}

if(bot->listHosts(tmpUser,theClient))
	{
	bot->Notice(theClient,"End of hosts for user %s",st[1].c_str());
	}
else
	{
	bot->Notice(theClient,"Error while accessing %s hostlist",st[1].c_str());
	}

delete tmpUser;
return true;
}
}