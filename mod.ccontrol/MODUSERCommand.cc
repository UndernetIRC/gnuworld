/**
 * MODUSERCommand.cc
 * Modify an oper database entry
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
 * $Id: MODUSERCommand.cc,v 1.28 2009/08/28 18:14:44 hidden1 Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"ccUser.h"
#include	"misc.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: MODUSERCommand.cc,v 1.28 2009/08/28 18:14:44 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool MODUSERCommand::Exec( iClient* theClient, const string& Message)
{	 
StringTokenizer st( Message ) ;

if(st.size() < 2)
	{
	Usage(theClient);
	return false;
	}
//Fetch the oper data base entry
if(st[1].size() > 64)
	{
	bot->Notice(theClient,"Oper name can't be more than 64 characters");
	return false;
	}

ccUser* tmpUser = bot->GetOper(st[1]);

if(!tmpUser)
	{
        bot->Notice(theClient,"%s isn't on my access list",st[1].c_str());
        return false;
	}
//Check if the user got a higher or equal flags than the one he's trying to edit	
ccUser* tmpAuth = bot->IsAuth(theClient);
if(!tmpAuth)
	{ //we should never get here
	return false;
	}
bot->MsgChanLog("MODUSER %s\n",st[1].c_str());

unsigned int AdFlag = tmpAuth->getType(); //Get the admin flag
unsigned int OpFlag = tmpUser->getType(); //Get the oper flag
bool Admin = (AdFlag < operLevel::SMTLEVEL);
bool Same = (tmpUser->getID() == tmpAuth->getID());

if((Admin) && (AdFlag <= OpFlag) && (!Same))
	{
	bot->Notice(theClient,"You can't modify a user who has a higher or "
		"equal level to your own.");
	return false;
	}
else if(AdFlag < OpFlag)
	{
	bot->Notice(theClient,"You can't modify a user who has a higher level "
		"than your own.");
	return false;
	}
if((Admin) && (strcasecmp(tmpAuth->getServer().c_str(),tmpUser->getServer().c_str())))
	{
	bot->Notice(theClient,"You can only modify a user who is associated to "
		"the same server as you.");
	return false;
	}
unsigned int pos = 2;
while(pos < st.size())
	{
	if(!strcasecmp(st[pos],"-p")) //Trying to change the password ?
		{
		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-p option must get new password");
			return false;
			}
		unsigned int passStat = bot->checkPassword(st[pos+1],tmpUser);
		switch(passStat)
			{
			case password::TOO_SHORT:
				bot->Notice(theClient,"Password must be atleast %d "
					"characters",
					password::MIN_SIZE);
				pos+=2;
				break;
			case password::LIKE_UNAME:
				bot->Notice(theClient,"Password can't be the same as the username");
				pos+=2;
				break;
			case password::PASS_OK:
				{
				tmpUser->setPassword(bot->CryptPass(st[pos+1]));
				tmpUser->setLast_Updated_By(theClient->getRealNickUserHost());
				tmpUser->setPassChangeTS(::time(0));
				if(tmpUser->Update())
					{
					bot->Notice(theClient,"Password for %s has been changed to %s",
						st[1].c_str(),st[pos+1].c_str());
					}
				else
					{
					bot->Notice(theClient,"Error while changing password for %s",st[1].c_str());
					}
					pos+=2;
				}
			}
		}			
	else if(!strcasecmp(st[pos],"-ah")) //Trying to add a new host ?
		{
		if((Same) && (AdFlag < operLevel::CODERLEVEL))
			{
			bot->Notice(theClient,"You can't add yourself another host");
			pos+=2;
			continue;
			}			

		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-ah option must get new hostmask");
			return false;
			}
		if(st[pos + 1].size() > 128)
			{
			bot->Notice(theClient,"Hostname can't be more than 128 characters");
			return false;
			}
		if(!bot->validUserMask(st[pos+1]))
			{
			bot->Notice(theClient,"Mask '%s' is not a valid mask in the form of *!*@*",
				st[pos+1].c_str());
			}
		else if(bot->UserGotHost(tmpUser,st[pos+1]))
			{
			bot->Notice(theClient,"'%s' already has the host (%s) listed.",
				st[1].c_str(),st[pos+1].c_str());
			}
		else if(bot->AddHost(tmpUser,st[pos+1]))
			{
			bot->Notice(theClient,"Mask %s added for %s",st[pos+1].c_str(),st[1].c_str());
			}
		else
			{
			bot->Notice(theClient,"Error while adding the mask (%s) for %s",
				st[pos+1].c_str(),st[1].c_str());
			}
		pos += 2;
		}
	else if(!strcasecmp(st[pos],"-dh")) //Trying to delete an host ?
		{
		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-dh option must get a host mask");
			return false;
			}
		if(st[pos + 1].size() > 128)
			{
			bot->Notice(theClient,"Hostname can't be more than 128 characters");
			return false;
			}
		if(!bot->UserGotHost(tmpUser,bot->removeSqlChars(st[pos+1])))
			{
			bot->Notice(theClient,"%s doesn't have the host '%s' in my access list",
				st[1].c_str(),st[pos+1].c_str());
			}
		else if(bot->DelHost(tmpUser,bot->removeSqlChars(st[pos+1])))
			{
			bot->Notice(theClient,"Mask '%s' was deleted from %s's access list",
				st[pos+1].c_str(),st[1].c_str());
			}
		else
			{
			bot->Notice(theClient,"Error while deleting mask '%s' from %s's access list",
				st[pos+1].c_str(),st[1].c_str());
			}
		pos += 2;
		}	    
	else if(!strcasecmp(st[pos],"-gl")) //Trying to toggle the get of logs
		{
		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-gl option must get on/off");
			return false;
			}
		if(!strcasecmp(st[pos+1],"on"))
			{
			// Only a smt+ can set getlogs to on for an oper
			if ((OpFlag < operLevel::ADMINLEVEL) && (AdFlag < operLevel::SMTLEVEL))
				{
				bot->Notice(theClient,"-gl can only be set for ADMINS+");
				return false;
				}
			tmpUser->setLogs(true);
			bot->Notice(theClient,"getLogs have been turned on for %s",st[1].c_str());
			}
		else if(!strcasecmp(st[pos+1],"off"))
			{
			tmpUser->setLogs(false);
			bot->Notice(theClient,"getLogs have been turned off for %s",st[1].c_str());
			}
		else
			{
			bot->Notice(theClient,"unknown option %s for -gl must be on/off",st[pos+1].c_str());
			return false;
			}
		tmpUser->setLast_Updated_By(bot->removeSqlChars(theClient->getRealNickUserHost()));
		tmpUser->Update();
		pos += 2;
		}	
	else if(!strcasecmp(st[pos],"-mt")) //Trying to change the message type
		{
		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-mt option must get message type");
			return false;
			}
		if((!strcasecmp(st[pos+1],"N")) || (!strcasecmp(st[pos+1],"NOTICE")))
			{
			tmpUser->setNotice(true);
			bot->Notice(theClient,"%s will now get NOTICES",st[1].c_str());
			}
		else if((!strcasecmp(st[pos+1],"M")) || (!strcasecmp(st[pos+1],"MESSAGE")))
			{
			tmpUser->setNotice(false);
			bot->Notice(theClient,"%s will now get MESSAGES",st[1].c_str());
			}
		else
			{
			bot->Notice(theClient,"unknown option %s for -mt must be n/m/notice/message",st[pos+1].c_str());
			return false;
			}
		tmpUser->setLast_Updated_By(bot->removeSqlChars(theClient->getRealNickUserHost()));
		tmpUser->Update();
		pos += 2;
		}	

	else if(!strcasecmp(st[pos],"-glag")) //Trying to toggle the get of lag reports
		{
		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-glag option must get on/off");
			return false;
			}
		if(!strcasecmp(st[pos+1],"on"))
			{
			tmpUser->setLag(true);
			bot->Notice(theClient,"getLag has been turned on for %s",st[1].c_str());
			}
		else if(!strcasecmp(st[pos+1],"off"))
			{
			tmpUser->setLag(false);
			bot->Notice(theClient,"getLag has been turned off for %s",st[1].c_str());
			}
		else
			{
			bot->Notice(theClient,"unknown option %s for -glag must be on/off",st[pos+1].c_str());
			return false;
			}
		tmpUser->setLast_Updated_By(bot->removeSqlChars(theClient->getRealNickUserHost()));
		tmpUser->Update();
		pos += 2;
		}	

	else if(!strcasecmp(st[pos],"-s")) //Trying to change the user server
		{
		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-s option must get a server");
			return false;
			}
		if(Admin)
			{
			bot->Notice(theClient,"Sorry, only SMT+ users can change an admins' server");
			return false;
			}
		if(st[pos + 1].size() > server::MaxName)
			{
			bot->Notice(theClient,"Server name can't be more than 128 characters");
			return false;
			}
		string SName = bot->expandDbServer(st[pos+1]);
		if(!strcasecmp(SName,""))
			{
			bot->Notice(theClient,"I can't see a server in the database that matches %s",
				    st[pos+1].c_str());
			return false;
			}
		if(!strcasecmp(tmpUser->getServer(),SName))
			{
			bot->Notice(theClient,"%s already is associated with %s",SName.c_str(),SName.c_str());
			pos+=2;
			}
		else
			{
			tmpUser->setServer(bot->removeSqlChars(SName));
			tmpUser->setLast_Updated_By(bot->removeSqlChars(theClient->getRealNickUserHost()));
			if(tmpUser->Update())
				{
				bot->Notice(theClient,"%s has been associated with %s",st[1].c_str(),SName.c_str());
				}
			else
				{
				bot->Notice(theClient,"Error while associating %s with %s",st[1].c_str(),SName.c_str());
				return false;
				}
			pos += 2;
			}	
		}		
	else if(!strcasecmp(st[pos],"-op")) //Trying to toggle needop
		{
		if(Admin)
			{
			bot->Notice(theClient,"Sorry, the needop flag is required.");
			pos+=2;
			continue;
			}
		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-op option must get on/off");
			return false;
			}
		if(!strcasecmp(st[pos+1],"on"))
			{
			tmpUser->setNeedOp(true);
			bot->Notice(theClient,"NeedOp has been turned on for %s",st[1].c_str());
			}
		else if(!strcasecmp(st[pos+1],"off"))
			{
			tmpUser->setNeedOp(false);
			bot->Notice(theClient,"NeedOp has been turned off for %s",st[1].c_str());
			}
		else
			{
			bot->Notice(theClient,"unknown option %s for -no must be on/off",st[pos+1].c_str());
			}
		tmpUser->setLast_Updated_By(bot->removeSqlChars(theClient->getRealNickUserHost()));
		tmpUser->Update();
		pos += 2;
		}	
	else if(!strcasecmp(st[pos],"-ua")) //Trying to update the access?
		{
		if((AdFlag < operLevel::CODERLEVEL) && (AdFlag <= OpFlag))
			{
			bot->Notice(theClient,"You can't update an access for a user who has a higher "
				"or equal level to your own.");
			}
		else
			{
			tmpUser->updateAccessFromFlags();
			tmpUser->setLast_Updated_By(bot->removeSqlChars(theClient->getRealNickUserHost()));
			if(tmpUser->Update())
				{
				bot->Notice(theClient,"Successfully updated access level for %s.",st[1].c_str());
				}
			else
				{
				bot->Notice(theClient,"Error while updating access level for %s.",st[1].c_str());
				}
	    		}
		pos++;
		}
	else if(!strcasecmp(st[pos],"-uf")) //Trying to update the user flags
		{
		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-uf option must get an argument.");
			return false;
			}
		unsigned int NewF;
		if(!strcasecmp(st[pos+1],"CODER"))
			{
			NewF = operLevel::CODERLEVEL;
			}
		else if(!strcasecmp(st[pos+1],"SMT"))
			{
			NewF = operLevel::SMTLEVEL;
			}
		else if(!strcasecmp(st[pos+1],"ADMIN"))
			{
			NewF = operLevel::ADMINLEVEL;
			}
		else if(!strcasecmp(st[pos+1],"OPER"))
			{
			NewF = operLevel::OPERLEVEL;
			}
		else
			{
			bot->Notice(theClient,"Bad option for -uf, must be CODER, SMT, ADMIN or OPER");
			NewF = 0;
			}
		if(NewF > 0)
			{
			if((AdFlag < operLevel::CODERLEVEL) && (AdFlag <= NewF))
				{
				bot->Notice(theClient,"You can't update the flags to a higher or equal "
					"level than your own.");
				}
			else
				{
				tmpUser->setType(NewF);
				tmpUser->setLast_Updated_By(theClient->getRealNickUserHost());
				if(tmpUser->Update())
					{
					bot->Notice(theClient,"Successfully updated flags for %s.",st[1].c_str());
					}
				else
					{
					bot->Notice(theClient,"Error while updating flags for %s.",st[1].c_str());
					}
				}
			}
		pos+=2;
		}
		
	else if(!strcasecmp(st[pos],"-e")) //Trying to change email
		{
		if((pos + 1) >= st.size())
			{
			bot->Notice(theClient,"-e option requires an e-mail address.");
			return false;
			}
		tmpUser->setEmail(bot->removeSqlChars(st[pos+1]));
		tmpUser->setLast_Updated_By(bot->removeSqlChars(theClient->getRealNickUserHost()));
		if(tmpUser->Update())
			{
			bot->Notice(theClient,"Successfully updated e-mail address for %s.",st[1].c_str());
			}
		else
			{
			bot->Notice(theClient,"Error while updating e-mail address for %s.",st[1].c_str());
			}
		pos+=2;
		}
	else
		{
		bot->Notice(theClient,"Unknown option %s",st[pos].c_str());
		pos++;
		}
	}
return true;
}		    				

}
}
