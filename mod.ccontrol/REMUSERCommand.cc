/*
 * REMUSERCommand.cc
 *
 * Removes an oper from the bot access list
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include        "Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char REMUSERCommand_cc_rcsId[] = "$Id: REMUSERCommand.cc,v 1.6 2001/12/13 08:50:00 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool REMUSERCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}
//Fetch the user record from the database
ccUser* theUser = bot->GetOper(bot->removeSqlChars(st[1]));
if (!theUser) 
	{ 
	bot->Notice(theClient,"Oper %s does not exists db," 
	"check your handle and try again",st[1].c_str());
	return false;
	}	    
ccUser* tempAuth = bot->IsAuth(theClient);
	
if(tempAuth->getType() < theUser->getType())
	{
	bot->Notice(theClient,"You cant remove an oper who got higer access than yours");
	return false;
	}
if(bot->DeleteOper(string_lower(st[1])))     
	{    
	bot->Notice(theClient,"Successfully Deleted Oper %s ",st[1].c_str());
	
	//Check if the user is authenticate 
	if(theUser->getClient())
		{
		//Get hte user iClient entry from the network , and notify him that he was deleted
		const iClient *TClient = theUser->getClient(); 
		if(TClient)
			bot->Notice(TClient,"You have been removed from my access list");
		//Remove the user authenticate entry
		bot->deAuthUser(theUser);
		}	
	delete theUser;
	return true;	
	}
else
	{    
	bot->Notice(theClient,"Error While Deleting Oper %s ",st[1].c_str());
	return false;	
	}
}

}
}

