/**
 * GLINECommand.cc
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
 * $Id: GLINECommand.cc,v 1.64 2009/06/09 05:55:55 hidden1 Exp $
 */

#include	<string>
#include        <iomanip>
#include	<map>

#include	<cstdlib>

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
#include	"gnuworld_config.h"

RCSTAG( "$Id: GLINECommand.cc,v 1.64 2009/06/09 05:55:55 hidden1 Exp $" ) ;

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

ccUser* tmpUser = bot->IsAuth(theClient);

bot->MsgChanLog("GLINE %s\n",st.assemble(1).c_str());

if(st[pos].substr(0,1) == "#")
{
	bot->Notice(theClient, "Please use the CHANGLINE command to gline channels");
	return true;
}

if(st[pos].find_first_of('#') != string::npos)
	{
	bot->Notice(theClient,"Nice try, but i dont think glining that host is such a good idea");
	return true;
	}
string userName;
string hostName;
	if(st[pos].substr(0,1) == "$")
		{
		bot->Notice(theClient,"Please use SGLINE to set this gline");
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
				bot->Notice( theClient, "i can't find '%s' online, "
					    "please specify a host instead",
						st[pos].c_str());
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
				string newMsg = "GLINE mask for " + st[pos] + " is " + userName + "@" + hostName;
				bot->MsgChanLog("%s\n",newMsg.c_str());
#ifndef LOGTOHD
				if(tmpUser)
					bot->DailyLog(tmpUser,"%s",newMsg.c_str());
				else
		    			bot->DailyLog(theClient,"%s",newMsg.c_str());
#else
				ccLog* newLog = new (std::nothrow) ccLog();
				newLog->Time = ::time(0);
				newLog->Desc = newMsg.c_str();
				newLog->Host = theClient->getRealNickUserHost().c_str();
				if(tmpUser)
					newLog->User = tmpUser->getUserName().c_str();
				else
					newLog->User = "Unknown";			
				newLog->CommandName = "GLINE";
				bot->DailyLog(newLog);
#endif
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
unsigned int ResStart = 2;
if (IsTimeSpec(st[2]))
{
	gLength = extractTime( st[2], 1 );
} else {
	gLength = bot->getDefaultGlineLength();
	ResStart = 1;
}
	
string nickUserHost = theClient->getRealNickUserHost() ;
	
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
			bot->Notice(theClient,"You can't gline for a negative amount of time.");
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
		if (gCheck & gline::BAD_CIDRLEN)
			{
			bot->Notice(theClient,"Bad CIDR length - try something more specific.");
			Ok = false;
			}
		if (gCheck & gline::BAD_CIDROVERRIDE)
			{
			bot->Notice(theClient,"For CIDR glines, you must enter all 4 parts of the IP.");
			Ok = false;
			}
		if(gCheck & gline::BAD_TIME)
			{
			bot->Notice(theClient,"Glining for more than %d seconds is not allowed.",
				gline::MFGLINE_TIME);
			Ok = false;
			}
		if((gCheck & gline::FORCE_NEEDED_HOST) && (Ok))
			{	
			bot->Notice(theClient,"Please use FORCEGLINE to gline that host");
			Ok = false;
			}
		if((gCheck & gline::FORCE_NEEDED_TIME) && (Ok))
		    	{
			bot->Notice(theClient,"Please use FORCEGLINE to gline for that amount of time");
			Ok = false;
			}
		if((gCheck & gline::FU_NEEDED_USERS) && (Ok))
			{
			bot->Notice(theClient,"This gline would affect more than %d users, please "
				"use FORCEGLINE",gline::MFGLINE_USERS);
			Ok = false;
			}
		if((gCheck & gline::FU_NEEDED_TIME) && (Ok))
			{
			bot->Notice(theClient,"Please use FORCEGLINE to gline for more than "
				"%d seconds",gline::MFGLINE_TIME);
			Ok = false;
			}
		if((gCheck & gline::FORCE_NEEDED_WILDTIME) && (Ok))
			{
			bot->Notice(theClient,"Wildcard gline for more than %d"
			    " seconds (or more than %d without ident), "
			    "please use FORCEGLINE instead"
			    ,gline::MGLINE_WILD_TIME ,
			    gline::MGLINE_WILD_NOID_TIME);
			Ok = false;
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
		if(!Ok)
			{
			bot->Notice(theClient,"Please fix all of the above, and try again");
			return false;
			}
		}
	char Us[100];
	Us[0] = '\0';
	sprintf(Us,"%d",Users);
	StringTokenizer reasonTokenizer(st.assemble( pos + ResStart),'|');
	string Reason = reasonTokenizer[0];
	if(Reason.size() > gline::MAX_REASON_LENGTH)
		{
		bot->Notice(theClient,"Gline reason can't be more than %d characters",
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

}

}
}
