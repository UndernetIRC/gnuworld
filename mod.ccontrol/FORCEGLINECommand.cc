/*
 * FORCECommand.cc
 *
 * Glines a specific mask 
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

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

const char FORCEGLINECommand_cc_rcsId[] = "$Id: FORCEGLINECommand.cc,v 1.8 2001/08/13 15:10:53 mrbean_ Exp $";

namespace gnuworld
{


using std::string ;

// Input: forcegline *@blah.net reason
// Input: forcegline 3600 *@blah.net reason
//
// Output: C GL * +*@lamer.net 3600 :Banned (*@lamer) ...
//
namespace uworld
{

bool FORCEGLINECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

StringTokenizer::size_type pos = 1 ;

bool Forced = false;

if(!strcasecmp(st[pos],"-fu"))
	{
	Forced = true;
	pos++;
	if( st.size() < 4 )
		{
		Usage( theClient ) ;
		return true ;
		}
	}
time_t gLength = bot->getDefaultGlineLength() ;

// (pos) is the index of the next token, the user@host mask.

string::size_type atPos = st[ pos ].find_first_of( '@' ) ;
if( string::npos == atPos )
	{
	// User has only specified hostname, not a user name
	bot->Notice( theClient, "GLINE: Please specify gline mask in the "
		"format: user@host" ) ;
	return true ;
	}

string userName = st[ pos ].substr( 0, pos ) ;
string hostName = st[ pos ].substr( pos + 1 ) ;
string Length;
Length.assign(st[pos+1]);
unsigned int Units = 1; //Defualt for seconds
unsigned int ResStart = 1;
bool Ok = true;
if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"d"))
	{
	Units = 24*3600;
	Length.resize(Length.length()-1);
	ResStart = 2;
	}
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"h"))
	{
	Units = 3600;
	Length.resize(Length.length()-1);
	ResStart = 2;
	}
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"s"))
	{
	Units = 1;
	Length.resize(Length.length()-1);
	ResStart = 2;
	}
gLength = atoi(Length.c_str()) * Units;

if(gLength == 0) 
	{
	gLength = bot->getDefaultGlineLength() ;
	bot->Notice(theClient,"No duration was set, setting to %d seconds by default",gLength) ;
	ResStart = 1;
	}
AuthInfo *tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
if(!tmpAuth)
	{ // We shouldnt have got here in the first place, but check it anyway
	return false;
	}
if((Forced) && (tmpAuth->getFlags() < operLevel::SMTLEVEL))
	{
	bot->Notice(theClient,"Only smt+ can use the -fu option");
	}
	
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
if((gCheck & gline::FORCE_NEEDED_USERS) && (Ok))
	{
	if(Forced)
		{
		bot->MsgChanLog("%s is using the force flag to gline %d users under the host of (%s@%s)"
		,theClient->getNickName().c_str()
		,Users,userName.c_str(),hostName.c_str());
		}
	else
		{
		Ok = false;
		if(tmpAuth->getFlags() < operLevel::SMTLEVEL)
			{
			bot->Notice(theClient,"Sorry but you cant set a gline which affects more than %d users"
			,gline::MFGLINE_USERS);
			}
		else
			{
			bot->Notice(theClient,"This gline affects more than %d users, please use the -fu flag"
			,gline::MFGLINE_USERS);
			}
		
		}
	}
if((gCheck & gline::FORCE_NEEDED_HOST) && (Ok))
	{	
	bot->MsgChanLog("%s is using forcegline to gline a forbidden host (%s@%s)"
	,theClient->getNickName().c_str()
	,userName.c_str(),hostName.c_str());
	}
if((gCheck & gline::FORCE_NEEDED_TIME) && (Ok))
	{
	bot->MsgChanLog("%s is using forcegline to gline for %s seconds"
	,gLength);
	}
if(gCheck & gline::FORCE_NEEDED_WILDTIME)
	{
	bot->MsgChanLog("%s is using forcegline to gline a wildcard host for more than %d seconds"
	,theClient->getNickName().c_str()
	,gline::MGLINE_WILD_TIME);
	}
if(!Ok)
	{
	bot->Notice(theClient,"Please fix all of the above, and try again");
	return false;
	}


// Avoid passing a reference to a temporary variable.
string nickUserHost = theClient->getNickUserHost() ;
string Reason = st.assemble( pos + ResStart );
char Us[100];
Us[0] = '\0';
sprintf(Us,"%d",Users);
if(Reason.size() > 255)
	{
	bot->Notice(theClient,"Gline reason can't be more than 255 chars");
	return false;
	}
server->setGline( nickUserHost,
	st[ pos ],
	st.assemble( pos + ResStart ) + "[" + Us + "]",
	gLength ) ;

ccGline *TmpGline = bot->findGline(st[pos]);
bool Up = false;

if(TmpGline)
	Up =  true;	
else TmpGline = new ccGline(bot->SQLDb);
TmpGline->setHost(st [ pos ]);
TmpGline->setExpires(::time(0) + gLength);
TmpGline->setAddedBy(nickUserHost);
TmpGline->setReason(st.assemble( pos + ResStart ));
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
	}
if(!Up)
	bot->addGline(TmpGline);
return true ;
}

}
}
