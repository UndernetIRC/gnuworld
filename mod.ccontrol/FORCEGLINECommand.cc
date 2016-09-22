/**
 * FORCECommand.cc
 * Glines a specific mask 
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
 * $Id: FORCEGLINECommand.cc,v 1.39 2009/06/09 05:55:55 hidden1 Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"misc.h"
#include	"Gline.h"
#include	"ip.h"
#include	"ELog.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: FORCEGLINECommand.cc,v 1.39 2009/06/09 05:55:55 hidden1 Exp $" ) ;

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
string cmdStr = "FORCEGLINE ";

ccUser* tmpUser = bot->IsAuth(theClient);
//bot->MsgChanLog("FORCEGLINE %s\n",st.assemble(1).c_str());
if(!strcasecmp(st[pos],"-fu"))
	{
	Forced = true;
	pos++;
	cmdStr += "-fu ";
	if( st.size() < 4 )
		{
		Usage( theClient ) ;
		return true ;
		}
	}
time_t gLength = bot->getDefaultGlineLength() ;

// (pos) is the index of the next token, the user@host mask.

if(string::npos != st[pos].find_first_of('#'))
	{
	bot->Notice(theClient,"I don't think glining that host is such a good idea, lose the #");
	return true;
	}
if(st[pos].substr(0,1) == "$")
	{
	bot->Notice(theClient,"Please use SGLINE to set that gline");
	return true;
	}
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
unsigned int ResStart = 2;
bool Ok = true;
if (IsTimeSpec(st[2]))
{
	gLength = extractTime( st[2], 1 );
} else {
	gLength = bot->getDefaultGlineLength() ;
	ResStart = 1;
}
//ccUser *tmpAuth = bot->IsAuth(theClient);
if(!tmpUser)
	{ // We shouldnt have got here in the first place, but check it anyway
	return false;
	}
if((Forced) && (tmpUser->getType() < operLevel::SMTLEVEL))
	{
	bot->Notice(theClient,"Only SMT+ can use the -fu option");
	return false;
	}
	
unsigned int Users;
string gHost = st[pos];
int gCheck = bot->checkGline(gHost,gLength,Users);
hostName = gHost;
cmdStr += gHost + " " + st.assemble(pos+1);

if(gCheck & gline::NEG_TIME)
	{
	bot->Notice(theClient,"You can't gline for a negative amount of time!");
	Ok = false;
	}	

if(gCheck & gline::HUH_NO_HOST)
	{
	bot->Notice(theClient,"I don't think glining that host is such a good idea, do you?");
	Ok = false;
	}
if(gCheck & gline::BAD_HOST)
	{
	bot->Notice(theClient,"illegal host");
	Ok = false;
	}
if(gCheck & gline::BAD_CIDRMASK)
        {
        bot->Notice(theClient,"The IP listed is not on a bit boundary for the CIDR mask specified.");
        Ok = false;
        }
if(gCheck & gline::BAD_CIDRLEN)
        {
        bot->Notice(theClient,"Bad CIDR length - try something more specific.");
        Ok = false;
        }
if(gCheck & gline::BAD_CIDROVERRIDE)
        {
        bot->Notice(theClient,"For CIDR glines, you must enter all 4 parts of the IP.");
        Ok = false;
        }
if(gCheck & gline::BAD_TIME)
	{
	bot->Notice(theClient,"Glining for more than %d seconds is not allowed",gline::MFGLINE_TIME);
	Ok = false;
	}
if((gCheck & gline::FU_NEEDED_USERS) && (Ok))
	{
	if(!Forced)
		{
		Ok = false;
		if(tmpUser->getFlags() < operLevel::SMTLEVEL)
			{
			bot->Notice(theClient,"Sorry, you can't set a gline which affects more than %d users",
				gline::MFGLINE_USERS);
			}
		else
			{
			bot->Notice(theClient,"This gline affects more than %d users, please use the -fu flag",
				gline::MFGLINE_USERS);
			}
		
		}
	}
if((gCheck & gline::FU_NEEDED_TIME) && (Ok))
	{
	if(!Forced)
		{
		Ok = false;
		if(tmpUser->getFlags() < operLevel::SMTLEVEL)
			{
			bot->Notice(theClient,"Sorry, you can't set a gline for more than %d seconds",
				gline::MFGLINE_TIME);
			}
		else
			{
			bot->Notice(theClient,"This gline is for more than %d seconds, please use the -fu flag",
				gline::MFGLINE_TIME);
			}
		}
	}
if((gCheck & gline::HUH_IS_IP_OF_OPER) && (Ok))
	{
	if (bot->isGlinedException(userName + "@" + hostName) > 0) {
		bot->Notice(theClient,"There is someone who previously opered from that host. G-line sent (forced)");
	}
	else {
		bot->Notice(theClient,"There is someone who previously opered from that host (%s). Send the gline again to force.", bot->getLastNUHOfOperFromIP(hostName).c_str());
		bot->addGlinedException(userName + "@" + hostName);
		Ok = false;
	}
	}
if((gCheck & gline::HUH_IS_EXCEPTION) && (Ok))
	{
	if (bot->isGlinedException(userName + "@" + hostName) > 0) {
		bot->Notice(theClient,"There is an exception for that host. G-line sent (forced)");
	}
	else {
		bot->Notice(theClient,"There is an exception for that host. Send the gline again to force.");
		bot->addGlinedException(userName + "@" + hostName);
		Ok = false;
	}
	}

/*if((gCheck & gline::FORCE_NEEDED_HOST) && (Ok))
	{	
	bot->MsgChanLog("%s is using forcegline to gline a wildcard host (%s@%s)"
	,theClient->getNickName().c_str()
	,userName.c_str(),hostName.c_str());
	}


if((gCheck & gline::FORCE_NEEDED_TIME) && (Ok))
	{
	bot->MsgChanLog("%s is using forcegline to gline for %d seconds"
	,theClient->getNickName().c_str(),gLength);
	}

if(gCheck & gline::FORCE_NEEDED_WILDTIME)
	{
	bot->MsgChanLog("%s is using forcegline to gline a wildcard host for more than %d seconds"
	,theClient->getNickName().c_str()
	,gline::MGLINE_WILD_TIME);
	}*/
if(!Ok)
	{
	bot->Notice(theClient,"Please fix all of the above, and try again");
	return false;

	}
if(gCheck & gline::GLINE_OK)
	{
	bot->Notice(theClient,"Please use the GLINE command to set that gline");
	return false;
	}
// Avoid passing a reference to a temporary variable.
string nickUserHost = theClient->getRealNickUserHost() ;
char Us[100];
Us[0] = '\0';
sprintf(Us,"%d",Users);
string Reason = st.assemble( pos + ResStart );
StringTokenizer ReasonTokenizer(Reason,'|');
Reason = string("[") + Us + "] " + ReasonTokenizer[0];
if(Reason.size() > gline::MAX_REASON_LENGTH)
	{
	bot->Notice(theClient,"Gline reason can't be more than %d characters",
		    gline::MAX_REASON_LENGTH);
	return false;
	}
/*server->setGline( nickUserHost,
	st[ pos ],
	string("[") + Us + "] " + Reason,
	//st.assemble( pos + ResStart ) + "[" + Us + "]",
	gLength , bot) ;*/

ccGline *TmpGline = bot->findGline(gHost);
bool Up = false;

if(TmpGline)
	Up =  true;	
else TmpGline = new ccGline(bot->SQLDb);
TmpGline->setHost(gHost);
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

bot->MsgChanLog(cmdStr.c_str());
return true ;
}

}
}
