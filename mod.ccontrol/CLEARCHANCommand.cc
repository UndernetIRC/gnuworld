/*
 * CLEARCHANCommand.cc 
 *
 * Clears all/some channel modes
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char CLEARCHANCommand_cc_rcsId[] = "$Id: CLEARCHANCommand.cc,v 1.4 2001/04/30 23:44:42 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;


bool CLEARCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s",
		st[ 1 ].c_str() ) ;
	return true ;
	}

string doModes; //This holds the modes the user asked to be removed
string remModes = ""; //Holds the modes that we are removing
string args = ""; //Holds the arguments for the remModes

//Check if the user specified the modes, if not assume he ment all of the modes
if(st.size() == 2)
	doModes = "OBKLI";
else if(!strcasecmp(string_upper(st[ 2 ]).c_str(),"ALL"))
	doModes = "OBKLINMSPT";
else	
	doModes = string_upper(st [ 2 ]);
	
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
			Channel::banListType::iterator ptr = theChan->banList_begin() ; 
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
					/* Don't deop +k things */
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
}
}
