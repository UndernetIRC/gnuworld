/*
 * REMOVEOPERCommand.cc
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

const char REMOVEOPERCommand_cc_rcsId[] = "$Id: REMOVEOPERCommand.cc,v 1.8 2001/07/20 09:09:31 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

bool REMOVEOPERCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}
//Fetch the user record from the database
ccUser* theUser = bot->GetOper(st[1]);
if (!theUser) 
	{ 
	bot->Notice(theClient,"Oper %s does not exists db," 
	"check your handle and try again",st[1].c_str());
	return false;
	}	    
AuthInfo* tempAuth = bot->IsAuth(theClient->getCharYYXXX());
	
if(bot->getTrueFlags(tempAuth->getFlags()) < theUser->getFlags())
	{
	bot->Notice(theClient,"You cant remove an oper who got higer access than yours");
	return false;
	}
if(bot->DeleteOper(string_lower(st[1])))     
	{    
	bot->Notice(theClient,"Successfully Deleted Oper %s ",st[1].c_str());
	
	//Check if the user is authenticate 
	AuthInfo *TDeauth = bot->IsAuth(theUser->getID());
	if(TDeauth)
		{
		//Get hte user iClient entry from the network , and notify him that he was deleted
		iClient *TClient = Network->findClient(TDeauth->getNumeric()); 
		if(TClient)
			bot->Notice(TClient,"You have been removed from my access list");
		//Remove the user authenticate entry
		bot->deAuthUser(TDeauth->getNumeric());
		}	
	delete theUser;
	return true;	
	}
else
	{    
	bot->Notice(theClient,"Error While Deleting Oper %s ",st[1].c_str());
	delete theUser;
	return false;	
	}
}
}