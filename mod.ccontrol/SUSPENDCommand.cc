/**
 * SUSPENDCommand.cc
 * Set oper as suspended
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: SUSPENDCommand.cc,v 1.15 2006/09/26 17:36:01 kewlio Exp $
 */

#include	<string>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"ccUser.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: SUSPENDCommand.cc,v 1.15 2006/09/26 17:36:01 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SUSPENDCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st( Message ) ;
	
if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}
//Fetch the oper record from the database	
ccUser* tmpUser = bot->GetOper(bot->removeSqlChars(st[1]));

if(!tmpUser)
	{
	bot->Notice(theClient,"%s isn't on my access list",st[1].c_str());
	return false;
	}
ccUser* tmpAuth = bot->IsAuth(theClient);
if(!tmpAuth)
	{ //we should never get here
	return false;
	}
bot->MsgChanLog("SUSPEND %s\n",st.assemble(1).c_str());

unsigned int AdFlag = tmpAuth->getType(); //Get the admin flag
unsigned int OpFlag = tmpUser->getType(); //Get the oper flag
bool Admin = AdFlag < operLevel::SMTLEVEL;

if((Admin) && (AdFlag <= OpFlag))
	{
	bot->Notice(theClient,"You can't suspend a user who has a higher or equal "
		"level to your own");
	return false;
	}
else if(AdFlag < OpFlag)
	{
	bot->Notice(theClient,"You can't suspend a user who has a higher level than "
		"your own");
	return false;
	}
if((Admin) && (strcasecmp(tmpAuth->getServer().c_str(),tmpUser->getServer().c_str())))
	{
	bot->Notice(theClient,"You can only suspend a user associated with the same server as you");
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
		bot->Notice(theClient,"Invalid suspend level must be OPER, ADMIN, SMT or CODER");
		return false;
		}
	if(Level > AdFlag)
		{
		bot->Notice(theClient,"You can't suspend at a level higher than your own!");
		return false;
		}
	ResPos = 5;
	}
//Set the suspention and update the db
tmpUser->setSuspendExpires(Len + time( 0 ));
tmpUser->setSuspendedBy(bot->removeSqlChars(theClient->getRealNickUserHost()));	    
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
	bot->Notice(theClient,"Error while suspending %s",st[1].c_str());
	return false;
	}

}

}
} // namespace gnuworld

