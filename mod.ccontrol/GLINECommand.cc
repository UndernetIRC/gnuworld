/*
 * GLINECommand.cc
 *
 * Glines a specific mask 
 */


#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
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
#include	"gline.h"
#include 	"time.h"
#include	"ccUser.h"

const char GLINECommand_cc_rcsId[] = "$Id: GLINECommand.cc,v 1.25 2001/11/08 23:13:29 mrbean_ Exp $";

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
if( st.size() < 4 )
	{
	Usage( theClient ) ;
	return true ;
	}

StringTokenizer::size_type pos = 1 ;

time_t gLength = bot->getDefaultGlineLength() ;

bool isChan;
if(st[pos].substr(0,1) == "#")
        isChan = true;
else
	isChan = false; 
if(!isChan)
	{
	string::size_type atPos = st[ pos ].find_first_of( '@' ) ;
	if( string::npos == atPos )
		{
		// User has only specified hostname, not a user name
		bot->Notice( theClient, "GLINE: Please specify gline mask in the "
			"format: user@host" ) ;
		return true ;
		}

	string userName = st[ pos ].substr( 0, atPos ) ;
	string hostName = st[ pos ].substr( atPos + 1 ) ;
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
	bot->Notice(theClient,"No duration was set, setting to %d seconds by default",gLength) ;
	ResStart = 1;
	}
string nickUserHost = theClient->getNickUserHost() ;
if(!isChan)
	{
	unsigned int Users;
	int gCheck = bot->checkGline(st[pos],gLength,Users);
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
		bot->Notice(theClient,"Please user forcegline to gline for more than %d second",gline::MFGLINE_TIME);
		Ok = false;
		}
	if((gCheck & gline::FORCE_NEEDED_WILDTIME) && (Ok))
		{
		bot->Notice(theClient,"Wildcard gline for more than %d seconds, must be set with forcegline",gline::MGLINE_WILD_TIME);
		Ok = false;
		}
	if(!Ok)
		{
		bot->Notice(theClient,"Please fix all of the above, and try again");
		return false;
		}
	char Us[100];
	Us[0] = '\0';
	sprintf(Us,"%d",Users);
	string Reason = st.assemble( pos + ResStart );
	if(Reason.size() > 255)
		{
		bot->Notice(theClient,"Gline reason can't be more than 255 chars");
		return false;
		}

	server->setGline( nickUserHost,
		st[ pos ],
		Reason + "[" + Us + "]",
		gLength ) ;
	
	ccGline *TmpGline = bot->findGline(st[pos]);
	bool Up = false;
	
	if(TmpGline)
		Up =  true;	
	else TmpGline = new ccGline(bot->SQLDb);
	TmpGline->setHost(bot->removeSqlChars(st [ pos ]));
	TmpGline->setExpires(::time(0) + gLength);
	TmpGline->setAddedBy(nickUserHost);
	TmpGline->setReason(bot->removeSqlChars(st.assemble( pos + ResStart )));
	TmpGline->setAddedOn(::time(0));
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
AuthInfo *tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
if(!tmpAuth)
	return false;
if(tmpAuth->getFlags() < operLevel::SMTLEVEL)
	{
	bot->Notice(theClient,"Only smt+ can use the gline #channel command");
	return false;
	}

typedef map<string , int> GlineMapType;
GlineMapType glineList;


if(st[1].size() > 200)
	{
	bot->Notice(theClient,"Channel name can't be more than 200 chars");
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
for( Channel::const_userIterator ptr = theChan->userList_begin();
ptr != theChan->userList_end() ; ++ptr )
	{
	TmpClient = ptr->second->getClient();
	GlineMapType::iterator gptr = glineList.find("*@" + TmpClient->getInsecureHost());
	if(gptr != glineList.end())
		{
		continue;
		}
	gptr = glineList.find("* " +TmpClient->getUserName() + "@" + TmpClient->getInsecureHost());		
	if(gptr != glineList.end())
		{
		continue;
		}
	if((!TmpClient->getMode(iClient::MODE_SERVICES)) 
	&& !(bot->IsAuth(theClient)) && !(TmpClient->isOper())) 
		{
		TmpGline = new ccGline(bot->SQLDb);
		assert(TmpGline != NULL);
		if(TmpClient->getUserName().substr(0,1) == "~")
			TmpGline->setHost("~*@" + TmpClient->getInsecureHost());
		else
			TmpGline->setHost("*" + TmpClient->getUserName() + "@" + TmpClient->getInsecureHost());
		TmpGline->setExpires(::time(0) + gLength);
		TmpGline->setAddedBy(nickUserHost);
		unsigned int Affected = Network->countMatchingUserHost(TmpGline->getHost()); 
		char Us[20];
		sprintf(Us,"%d",Affected);
		TmpGline->setReason(st.assemble( pos + ResStart ));
		TmpGline->setAddedOn(::time(0));
		TmpGline->Insert();
		TmpGline->loadData(TmpGline->getHost());
		bot->addGline(TmpGline);
		server->setGline( nickUserHost,
			    TmpGline->getHost(),
			    TmpGline->getReason() + "[" + Us + "]" ,
			    gLength ) ;
		glineList.insert(GlineMapType::value_type(TmpGline->getHost(),0));
		}
	}
			
return true ;
}

}
}
