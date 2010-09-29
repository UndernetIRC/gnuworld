/**
 * CLEARCHANCommand.cc 
 * Clears all/some channel modes
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
 * $Id: CLEARCHANCommand.cc,v 1.26 2006/09/26 17:35:58 kewlio Exp $
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"ccBadChannel.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: CLEARCHANCommand.cc,v 1.26 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::vector ;
using gnuworld::iClient;

namespace uworld
{

bool CLEARCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
bool Desynch = false;

if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[1].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d characters.",
		channel::MaxName);
	return false;
	}

Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s",
		st[ 1 ].c_str() ) ;
	return true ;
	}
if(bot->isOperChan(theChan))
	{
	bot->Notice(theClient,"C'mon, you know you can't clear an oper channel.");
	return false;
	}

string doModes; //This holds the modes the user asked to be removed
string remModes = ""; //Holds the modes that we are removing
string args = ""; //Holds the arguments for the remModes

bot->MsgChanLog("CLEARCHAN %s\n",st.assemble(1).c_str());
ccBadChannel* Chan = bot->isBadChannel(st[1]);
if(Chan)
        {
        bot->Notice(theClient,"Sorry, you can't change modes in "
                             "this channel because: %s",
                             Chan->getReason().c_str());
        return false;
        }

//Check if the user specified the modes, if not assume he ment all of the modes
if(st.size() == 2)
	doModes = "obklim";
else if(!strcasecmp(string_upper(st[ 2 ]),"ALL"))
	doModes = "obklimnspt";
else if(!strcasecmp(string_upper(st [ 2]),"-d"))
	Desynch = true;
else	
	doModes = st [ 2 ];

if(Desynch)
	{
	vector<iClient*> KickVec;
	bot->Join(theChan->getName(),"+i",0,true);
	for( Channel::const_userIterator ptr = theChan->userList_begin();
		ptr != theChan->userList_end() ; ++ptr )
			{
			
			if ( !ptr->second->getClient()->getMode(iClient::MODE_SERVICES) ) 
				{
				KickVec.push_back(ptr->second->getClient());
				}
			else
				{
				/* 
				 its a +k user, need to make sure its not us
				 */
				if(strcmp(ptr->second->getClient()->getCharYYXXX().c_str(),
					 bot->getCharYYXXX().c_str()))
					{ 
					bot->Message(ptr->second->getClient(),"OPERPART %s"
						    ,theChan->getName().c_str());
					}
				}
			}
	if(KickVec.size() > 0)
		{
		string reason = "Desynch clearing";
		bot->Kick(theChan,KickVec,reason);
		}
	bot->Part(theChan->getName());
	return true;
	}

bot->ClearMode( theChan, doModes, true ) ;
return true ;

/*	
for( string::size_type modePos = 0 ; modePos < doModes.size() ; ++modePos )
	{
	switch( doModes[ modePos ] )
		{
		case 'B':   //Ban?
			{
			string ban;
			string modes = "-";
			string args = "";
			Channel::banListSizeType end = theChan->banList_size();
			Channel::banIterator ptr = theChan->banList_begin() ; 
			for(Channel::banListSizeType i=0; i != end ;i++)
				{ 
				ban = *ptr;
				ptr++;
				args += ban + ' ';
				modes+= "b";
				theChan->removeBan(ban);
				if(modes.size() > 5) //if we got more than 5 , set the mode and continue
					{
					bot->ModeAsServer( theChan, modes + ' ' + args ) ;
					modes = "-";
					args = "";
					}
				}
			if(!args.empty())
				bot->ModeAsServer( theChan, modes + ' ' + args ) ;
			break;
			}
		case 'O':  //Chanops?
			{
			string modes = "-";
			string args = "";
			for( Channel::const_userIterator ptr = theChan->userList_begin();
			ptr != theChan->userList_end() ; ++ptr )
				{
				if( ptr->second->getMode(ChannelUser::MODE_O))
					{
					// Don't deop +k things
					if ( !ptr->second->getClient()->getMode(iClient::MODE_SERVICES) ) 
						{
						modes+= 'o';
						args+= ptr->second->getCharYYXXX() + " ";
						ptr->second->removeMode(ChannelUser::MODE_O);
						}
					} // If opped.
				if(modes.size() > 5) //if we got more than 5 , set the mode and continue
					{
					bot->ModeAsServer( theChan, modes + ' ' + args ) ;
					modes = "-";
					args = "";
					}
				}
			if(!args.empty())
				bot->ModeAsServer(theChan,modes + " " + args);
			}
			break;
		case 'K':  //Key?
			if(theChan->getMode(Channel::MODE_K))
				{
				theChan->removeMode(Channel::MODE_K);
				remModes+= "k";
				args+= theChan->getKey() + " ";
				theChan->setKey("");
				}
			break;
		case 'I':  //Invite?
			if(theChan->getMode(Channel::MODE_I))
				{
				theChan->removeMode(Channel::MODE_I);
				remModes+= "i";
				}
			break;
		case 'L': //Limit?
			if(theChan->getMode(Channel::MODE_L))
				{
				theChan->removeMode(Channel::MODE_L);
				remModes+= "l";
				//args+= theChan->getLimit() + " ";
				}
			break;
		case 'P':  //Private?
			if(theChan->getMode(Channel::MODE_P))
				{
				theChan->removeMode(Channel::MODE_P);
				remModes+= "p";
				}
			break;
		case 'S':  //Secret?
			if(theChan->getMode(Channel::MODE_S))
				{
				theChan->removeMode(Channel::MODE_S);
				remModes+= "s";
				}
			break;
		case 'M':  //Moderated?
			if(theChan->getMode(Channel::MODE_M))
				{
				theChan->removeMode(Channel::MODE_M);
				remModes+= "m";
				}
			break;
		case 'N':  //No External Messages?
			if(theChan->getMode(Channel::MODE_N))
				{
				theChan->removeMode(Channel::MODE_N);
				remModes+= "n";
				}
			break;
		case 'T':  //Topic?
			if(theChan->getMode(Channel::MODE_T))
				{
				theChan->removeMode(Channel::MODE_T);
				remModes+= "t";
				}
			break;
		default:;
	}	}			
if(!remModes.empty())
	bot->ModeAsServer(theChan,"-" + remModes + " " + args);
return true;	
*/
}

}
}
