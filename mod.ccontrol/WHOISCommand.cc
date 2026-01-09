/**
 * WHOISCommand.cc
 * Gives varius data about a user
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
 * $Id: WHOISCommand.cc,v 1.25 2005/08/24 13:36:32 kewlio Exp $
 */
#include	<string>
#include	<iostream>
#include	<cstdlib>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"ip.h"
#include	"ccontrol_generic.h"
#include	"gnuworld_config.h"

namespace gnuworld
{
using std::endl ;
using std::string ;

namespace uworld
{

// whois nickname
bool WHOISCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

iClient* Target = Network->findNick( st[ 1 ] ) ;
if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find nick: %s", st[ 1 ].c_str() ) ;
	return true ;
	}
bot->MsgChanLog("WHOIS %s\n",st.assemble(1).c_str());

iServer* targetServer = Network->findServer( Target->getIntYY() ) ;
if( NULL == targetServer )
	{
	elog	<< "WHOISCommand> Unable to find server: "
		<< Target->getIntYY() << endl ;
	return false ;
	}

if((Target->isModeX()) && (Target->isModeR()))
	{
	bot->Notice( theClient, "%s is %s (%s) [%s]",
		st[ 1 ].c_str(),
		Target->getNickUserHost().c_str(),
		Target->getRealNickUserHost().c_str(),
		xIP(Target->getIP() ).GetNumericIP().c_str()
	) ;
	
	}
else	
	{
	bot->Notice( theClient, "%s is %s [%s]",
		st[ 1 ].c_str(),
		Target->getNickUserHost().c_str(),
		xIP(Target->getIP() ).GetNumericIP().c_str()
		) ;
	}

if( Target->getNickTS() != Target->getFirstNickTS() )
	{
	bot->Notice(theClient, "%s has used its current nickname for %s [since %ld]",
		st[1].c_str(),
		Ago(Target->getNickTS()),
		Target->getNickTS());
	}

bot->Notice(theClient, "%s has been connected for %s [since %ld]",
	st[1].c_str(),
	Ago(Target->getFirstNickTS()),
	Target->getFirstNickTS());

if (Target->isModeR())
{
	string accountFlags;
	if( Target->getAccountFlag( iClient::X_TOTP_REQ_IPR ) || Target->getAccountFlag(iClient::X_TOTP_ENABLED ) )
		{
		accountFlags += ( Target->getAccountFlag( iClient::X_TOTP_REQ_IPR ) ) ? "TOTP_REQ_IPR " : "TOTP ";

		// Check for disabled methods
		std::string disabled;
		if( Target->getAccountFlag( iClient::X_WEB_DISABLE_TOTP ) )
			disabled += ( disabled.empty() ? "WEB" : ",WEB" ) ;
		if( Target->getAccountFlag( iClient::X_CERT_DISABLE_TOTP ) )
			disabled += ( disabled.empty() ? "CERT" : ",CERT" ) ;
		if( !disabled.empty() )
			accountFlags += "(DISABLE=" + disabled + ") ";
		}

	if(Target->getAccountFlag(iClient::X_GLOBAL_SUSPEND))
		accountFlags += "SUSPENDED ";
	if(Target->getAccountFlag(iClient::X_FRAUD))
		accountFlags += "FRAUD ";
	if(Target->getAccountFlag(iClient::X_CERTONLY))
		accountFlags += "CERTONLY ";

	/* client is authed - show it here */
	bot->Notice(theClient, "%s is authed as [%s]",
		st[1].c_str(),
		Target->getAccount().c_str());
	if( !accountFlags.empty())
		bot->Notice(theClient, "   Flags: %s", accountFlags.c_str());
}

bot->Notice( theClient, "Numeric: %s, UserModes: %s, Server Numeric: %s (%s)",
	Target->getCharYYXXX().c_str(),
	Target->getStringModes().c_str(),
	targetServer->getCharYY().c_str(),
	targetServer->getName().c_str()
	) ;

if( Target->isModeZ() )
	{
	bot->Notice( theClient, "%s is connected using TLS",
		st[ 1 ].c_str()) ;
	if( Target->hasTlsFingerprint() )
		bot->Notice( theClient, "   Fingerprint: %s", compactToCanonical( Target->getTlsFingerprint() ).c_str() ) ;
	}

if( Target->isOper() )
	{
	bot->Notice( theClient, "%s is an IRCoperator",
		st[ 1 ].c_str() ) ;
	}

if(Target->getMode(iClient::MODE_SERVICES))
	{
	return true;
	}
	
vector< string > channels ;
string curChannel ;
string::size_type curPlace;
string tChannel;
char curChar[10];
gnuworld::Channel* theChannel;
gnuworld::ChannelUser* theChannelUser;
bool hasCC; //Channel has control codes

for( iClient::const_channelIterator ptr = Target->channels_begin() ;
	ptr != Target->channels_end() ; ++ptr )
	{
	curChannel = "";
	tChannel = (*ptr)->getName();
        theChannel = (*ptr);
        theChannelUser = theChannel->findUser(Target);
        tChannel = theChannel->getName();
        if(theChannelUser->getMode(gnuworld::ChannelUser::MODE_V)) tChannel = "+" + tChannel;
        if(theChannelUser->getMode(gnuworld::ChannelUser::MODE_O)) tChannel = "@" + tChannel;
	if ((theChannel->getMode(Channel::MODE_S)) || (theChannel->getMode(Channel::MODE_P)))
		tChannel = "!" + tChannel;

	hasCC = false;
	for(curPlace = 0; curPlace < tChannel.size();++curPlace)
		{
		if(((tChannel[curPlace] > 1) && (tChannel[curPlace] < 4))
			|| (tChannel[curPlace] == 15) 
			|| ((tChannel[curPlace] > 27) && (tChannel[curPlace] < 33))
			|| (tChannel[curPlace] == 22)
			|| (tChannel[curPlace] == char(160))
                        || ((unsigned(tChannel[curPlace]) > 252)  
			&& (unsigned(tChannel[curPlace]) <= 254)))
			{			
			hasCC = true;
			sprintf(curChar,"%d",tChannel[curPlace]);
			curChannel += string("\x16^") + curChar + string("\x16");
			}
		else
			{
			curChannel += tChannel[curPlace];
			}
		}

	if(hasCC)
		{
		curChannel = string("*") + curChannel;
		}

	channels.push_back( curChannel) ;
	}

if( channels.empty() )
	{
	return true ;
	}

string chanNames ;
for( vector< string >::size_type i = 0 ; i < channels.size() ; i++ )
	{
	if ((chanNames.size() + channels[i].size()) > 410)
	{
		/* need to split lines up */
		bot->Notice(theClient, "On channels: %s", chanNames.c_str());
		chanNames = "";
	}
	chanNames += channels[ i ] ;
	if( (i + 1) < channels.size() )
		{
		chanNames += ", " ;
		}
	}

bot->Notice( theClient, "On channels: %s", chanNames.c_str() ) ;
bot->Notice( theClient, "* - Channel contains control codes" );
bot->Notice( theClient, "! - Channel is +s or +p" );

return true ;
}

}
}

