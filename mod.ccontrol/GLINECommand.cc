/*
 * GLINECommand.cc
 *
 * Glines a specific mask 
 */


#include	<string>
#include	<cstdlib>
#include        <iomanip>
#include	<map>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"misc.h"
#include	"Gline.h"
#include	"ip.h"
#include	"ELog.h"
#include	"Gline.h"
//#include	"gline.h"
#include 	"time.h"
#include	"ccUser.h"
#include	"Constants.h"

const char GLINECommand_cc_rcsId[] = "$Id: GLINECommand.cc,v 1.50 2003/05/19 08:45:06 mrbean_ Exp $";

namespace gnuworld
{


using std::string ;

// Input: gline *@blah.net reason
// Input: gline 3600 *@blah.net reason
//
// Output: C GL * +*@lamer.net 3600 :Banned (*@lamer) ...
//

namespace uworld
{

bool GLINECommand::Exec( iClient* theClient, const string& Message )
{
bool Ok = true;
StringTokenizer st( Message ) ;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if( st.size() < 4 )
	{
	Usage( theClient ) ;
	return true ;
	}

StringTokenizer::size_type pos = 1 ;


time_t gLength = bot->getDefaultGlineLength() ;

ccUser* tmpUser = bot->IsAuth(theClient);

bot->MsgChanLog("GLINE %s\n",st.assemble(1).c_str());

bool isChan;
if(st[pos].substr(0,1) == "#")
        isChan = true;
else if(st[pos].find_first_of('#') != string::npos)
	{
	bot->Notice(theClient,"Nice try, but i dont think glining that host is such a good idea");
	return true;
	}
else
	isChan = false; 
string userName;
string hostName;
if(!isChan)
	{
	if(st[pos].substr(0,1) == "$")
		{
		bot->Notice(theClient,"Please use sgline to set this gline");
		return true;
		}
	string::size_type atPos = st[ pos ].find_first_of( '@' ) ;
	if( string::npos == atPos )
		{
			
		if((atPos = st [ pos ].find_first_of('.')) == string::npos) 
			{
			// user has probably specified a nickname (asked by isomer:P)
			iClient* tClient = Network->findNick(st[pos]);
			if(!tClient)
				{
				bot->Notice( theClient, "i can't find %s online, "
					    "please specify a host instead",st[pos].c_str() ) ;
				return true ;
				}
			else   //Ohhh neat we found our target, lets grab his ip
				{
				userName = tClient->getUserName();
				if(userName[0] == '~')
					{
					userName = "~*";
					}
				hostName = xIP(tClient->getIP()).GetNumericIP();
				}
			}
		else
			{
			//user  forgot to add *@ so lets add it for him
			userName = "*";
			hostName = st[ pos  ];
			}
		}
	else
		{
		userName = st[ pos ].substr( 0, atPos ) ;
		hostName = st[ pos ].substr( atPos + 1 ) ;
		}
	}
string Length;

Length.assign(st[2]);
unsigned int Units = 1; //Defualt for seconds
unsigned int ResStart = 2;

if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"d"))
	{
	Units = 24*3600;
	Length.resize(Length.length()-1);
	}
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"h"))
	{
	Units = 3600;
	Length.resize(Length.length()-1);
	}
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"m"))
	{
	Units = 60;
	Length.resize(Length.length()-1);
	}
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"s"))
	{
	Units = 1;
	Length.resize(Length.length()-1);
	}
gLength = atoi(Length.c_str()) * Units;
if(gLength == 0) 
	{
	gLength = bot->getDefaultGlineLength() ;
	ResStart = 1;
	}
string nickUserHost = theClient->getRealNickUserHost() ;
	
if(!isChan)
	{
	unsigned int Users;
	if(!tmpUser)
		{
		if((string::npos != hostName.find_first_of("*")) 
		    || (string::npos != hostName.find_first_of("?")) 
		    || ((unsigned)gLength > gline::NOLOGIN_TIME))
			{
			bot->Notice(theClient,"You must login to issue this gline!");
			return true;
			}
		Users = Network->countMatchingRealUserHost(string(userName + "@" + hostName));
		}
	else
		{
		int gCheck = bot->checkGline(string(userName + "@" + hostName),gLength,Users);
		if(gCheck & gline::NEG_TIME)
			{
			bot->Notice(theClient,"Hmmz, dont you think that giving a negative time is kinda stupid?");
			Ok = false;
			}	
		if(gCheck & gline::HUH_NO_HOST)
			{
			bot->Notice(theClient,"I dont think glining that host is such a good idea, do you?");
			Ok = false;
			}
		if(gCheck & gline::BAD_HOST)
			{
			bot->Notice(theClient,"illegal host");
			Ok = false;
			}
		if(gCheck & gline::BAD_TIME)
			{
			bot->Notice(theClient,"Glining for more than %d seconds is a NoNo",gline::MFGLINE_TIME);
			Ok = false;
			}
		if((gCheck & gline::FORCE_NEEDED_HOST) && (Ok))
			{	
			bot->Notice(theClient,"Please use forcegline to gline that host");
			Ok = false;
			}
		if((gCheck & gline::FORCE_NEEDED_TIME) && (Ok))
		    	{
			bot->Notice(theClient,"Please use forcegline to gline for that amount of time");
			Ok = false;
			}
		if((gCheck & gline::FU_NEEDED_USERS) && (Ok))
			{
			bot->Notice(theClient,"This host affects more than %d users, please use forcegline",gline::MFGLINE_USERS);
			Ok = false;
			}
		if((gCheck & gline::FU_NEEDED_TIME) && (Ok))
			{
			bot->Notice(theClient,"Please use forcegline to gline for more than %d second",gline::MFGLINE_TIME);
			Ok = false;
			}
		if((gCheck & gline::FORCE_NEEDED_WILDTIME) && (Ok))
			{
			bot->Notice(theClient,"Wildcard gline for more than %d"
			    " seconds (or more than %d without ident)," 
			    "must be set with forcegline"
			    ,gline::MGLINE_WILD_TIME ,
			    gline::MGLINE_WILD_NOID_TIME);
			Ok = false;
			}
	    	if(!Ok)
			{
			bot->Notice(theClient,"Please fix all of the above, and try again");
			return false;
			}
		}
	char Us[100];
	Us[0] = '\0';
	sprintf(Us,"%d",Users);
	string Reason = st.assemble( pos + ResStart );
	if(Reason.size() > gline::MAX_REASON_LENGTH)
		{
		bot->Notice(theClient,"Gline reason can't be more than %d chars",
			    gline::MAX_REASON_LENGTH);
		return false;
		}
	Reason = string("[") + Us + string("] ") + Reason;
/*	server->setGline( nickUserHost,
		userName + "@" +hostName,
		string("[") + Us + "] " + Reason,
		//Reason + "[" + Us + "]",
		gLength ,::time(0),bot) ;*/
	ccGline *TmpGline = bot->findGline(userName + "@" + hostName);
	bool Up = false;
	
	if(TmpGline)
		Up =  true;	
	else TmpGline = new ccGline(bot->SQLDb);
	TmpGline->setHost(userName + "@" + hostName);
	TmpGline->setExpires(unsigned(::time(0) + gLength));
	TmpGline->setAddedBy(nickUserHost);
	TmpGline->setReason(Reason);
	TmpGline->setAddedOn(::time(0));
	TmpGline->setLastUpdated(::time(0));
	bot->addGlineToUplink(TmpGline);
	if(Up)
		{	
		TmpGline->Update();
		}
	else
		{
		TmpGline->Insert();
		//We need to update the Id
		TmpGline->loadData(TmpGline->getHost());
		bot->addGline(TmpGline);
		}

	return true;
	} //end of regular gline

//Its a channel gline
//ccUser *tmpAuth = bot->IsAuth(theClient);
if(!tmpUser)
	{
	bot->Notice(theClient,"You must login to issue this gline!");
	return false;
	}
if(tmpUser->getType() < operLevel::SMTLEVEL)
	{
	bot->Notice(theClient,"Only smt+ can use the gline #channel command");
	return false;
	}

typedef map<string , int> GlineMapType;
GlineMapType glineList;


if(st[1].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d chars",channel::MaxName);
	return false;
	}

Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s",
		st[ 1 ].c_str() ) ;
	return true ;
	}
ccGline *TmpGline;
iClient *TmpClient;
string curIP;
GlineMapType::iterator gptr;
for( Channel::const_userIterator ptr = theChan->userList_begin();
ptr != theChan->userList_end() ; ++ptr )
	{
	TmpClient = ptr->second->getClient();
	curIP = xIP(TmpClient->getIP()).GetNumericIP();
	gptr = glineList.find("*@" + curIP);
	if(gptr != glineList.end())
		{
		continue;
		}
	if((!TmpClient->getMode(iClient::MODE_SERVICES)) 
	&& !(bot->IsAuth(TmpClient)) && !(TmpClient->isOper())) 
		{
		TmpGline = new ccGline(bot->SQLDb);
		assert(TmpGline != NULL);
		TmpGline->setHost("*@"  + curIP);
		TmpGline->setExpires(unsigned(::time(0) + gLength));
		TmpGline->setAddedBy(nickUserHost);
		TmpGline->setReason(st.assemble( pos + ResStart ));
		TmpGline->setAddedOn(::time(0));
		TmpGline->setLastUpdated(::time(0));
		bot->queueGline(TmpGline);
		glineList.insert(GlineMapType::value_type(TmpGline->getHost(),0));
		}
	}
			
return true ;
}

}
}
