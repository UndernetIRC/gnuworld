/*
 * SUSPENDCommand.cc
 *
 * Set oper as suspended
 *
 */

#include	<string>
#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"ccUser.h"

const char SUSPENDCommand_cc_rcsId[] = "$Id: SUSPENDCommand.cc,v 1.7 2001/12/13 08:50:00 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SUSPENDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
	
if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}
//Fetch the oper record from the database	
ccUser* tmpUser = bot->GetOper(bot->removeSqlChars(st[1]));

if(!tmpUser)
	{
	bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
	return false;
	}
ccUser* tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
unsigned int AdFlag = tmpAuth->getType(); //Get the admin flag
unsigned int OpFlag = tmpUser->getType(); //Get the oper flag
bool Admin = AdFlag < operLevel::SMTLEVEL;

if((Admin) && (AdFlag <= OpFlag))
	{
	bot->Notice(theClient,"You cant suspend a user who got higher/equal level than yours");
	return false;
	}
else if(AdFlag < OpFlag)
	{
	bot->Notice(theClient,"You cant suspend a user who got higher level than yours");
	return false;
	}
if((Admin) && (strcasecmp(tmpAuth->getServer().c_str(),tmpUser->getServer().c_str())))
	{
	bot->Notice(theClient,"You can only suspend a user who's associated to the same server as you");
	return false;
	}

if(bot->isSuspended(tmpUser))
	{
	bot->Notice(theClient,"%s is already suspended",st[1].c_str());
	return false;
	}
//Fill in the suspendtion period according to the user entry	
unsigned int Units = 1;
string Length = st[2];
string Un = Length.substr(Length.length() - 1);
if(!strcasecmp(Un,"d"))
	{
	Units = 24*3600;
	Length.resize(Length.length()-1);
	}
else if(!strcasecmp(Un,"h"))
	{
	Units = 3600;
	Length.resize(Length.length()-1);
	}
else if(!strcasecmp(Un,"m"))
	{
	Units = 60;
	Length.resize(Length.length()-1);
	}
else if(!strcasecmp(Un,"s"))
	{
	Units = 1;
	Length.resize(Length.length()-1);
	}
unsigned int Len = atoi(Length.c_str()) * Units;
if(Len == 0)
	{
	bot->Notice(theClient,"Invalid duration!");
	return false;
	}
unsigned int Level = operLevel::OPERLEVEL;;
unsigned int ResPos = 3;
if(!strcasecmp(st[3],"-l"))
	{
	if(st.size() < 6)
		{
		Usage(theClient);
		return false;
		}
	if(!strcasecmp(st[4],"OPER"))
		{
		Level = operLevel::OPERLEVEL;
		}
	else if(!strcasecmp(st[4],"ADMIN"))
		{
		Level = operLevel::ADMINLEVEL;
		}
	else if(!strcasecmp(st[4],"SMT"))
		{
		Level = operLevel::SMTLEVEL;
		}
	else if(!strcasecmp(st[4],"CODER"))
		{
		Level = operLevel::CODERLEVEL;
		}
	else
		{
		bot->Notice(theClient,"Invalid suspend level must be OPER/ADMIN/SMT/CODER");
		return false;
		}
	if(Level > AdFlag)
		{
		bot->Notice(theClient,"You cant suspend with a level higher than yours!");
		return false;
		}
	ResPos = 5;
	}
//Set the suspention and update the db
tmpUser->setSuspendExpires(Len + time( 0 ));
tmpUser->setSuspendedBy(theClient->getNickUserHost());	    
tmpUser->setIsSuspended(true);
tmpUser->setSuspendLevel(Level);
tmpUser->setSuspendReason(bot->removeSqlChars(st.assemble(ResPos)));	
if(tmpUser->Update())
	{
	bot->Notice(theClient,"%s has been suspended",st[1].c_str());
	return true;
	}
else
	{
	bot->Notice(theClient,"Error while suspendeding %s",st[1].c_str());
	return false;
	}

}

}
} // namespace gnuworld

