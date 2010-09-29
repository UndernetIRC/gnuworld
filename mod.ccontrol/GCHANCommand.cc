/**
 * GCHANCommand.cc
 *
 * close down a channel
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
 * $Id: GCHANCommand.cc,v 1.17 2006/09/26 17:35:58 kewlio Exp $
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
#include	"Gline.h"
//#include	"gline.h"
#include 	"time.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: GCHANCommand.cc,v 1.17 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool GCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if( st.size() < 4 )
	{
	Usage( theClient ) ;
	return true ;
	}

StringTokenizer::size_type pos = 1 ;

bot->MsgChanLog("GCHAN %s\n",st.assemble(1).c_str());
time_t gLength = bot->getDefaultGlineLength() ;

// (pos) is the index of the next token, the user@host mask.

if( (st[pos].substr(0,1) != "#" ) || (st[pos].size() > channel::MaxName))
	{
	// Channel name must start with #
	bot->Notice( theClient, "GCHAN: Please specify a legal channel name "
		"(must start with # and no longer than %d characters)",
		channel::MaxName ) ;
	return true ;
	}

string Length;
Length.assign(st[pos+1]);
unsigned int Units = 1; //Defualt for seconds
unsigned int ResStart = 2;
bool isPerm = false;
if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"d"))
	{
	Units = 24*3600;
	Length.resize(Length.length()-1);
	//ResStart = 2;
	}
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"h"))
	{
	Units = 3600;
	Length.resize(Length.length()-1);
//	ResStart = 2;
	}
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"m"))
	{
	Units = 60;
	Length.resize(Length.length()-1);
//	ResStart = 2;
	}

else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"s"))
	{
	Units = 1;
	Length.resize(Length.length()-1);
//	ResStart = 2;
	}
gLength = atoi(Length.c_str()) * Units;
if(gLength == 0) 
	{ //ok its a string, check if it marks permanent gline
	if(!strcasecmp(Length,"-per"))
		{
		isPerm = true;
		gLength = gline::PERM_TIME; //Set for two years , its long enough
		if(st.size() < 4)
			{
			Usage(theClient);
			return false;
			}
		}
	else
		{
	gLength = bot->getDefaultGlineLength() ;
	bot->Notice(theClient,"No duration was set, using %d seconds by default",
		gLength);
	ResStart = 1;
		}
	}
else if(st.size() < 4)
	{
	Usage(theClient);
	return false;
	}

// Avoid passing a reference to a temporary variable.
string nickUserHost = theClient->getRealNickUserHost() ;
string Reason = st.assemble( pos + ResStart );
if(Reason.size() > 255)
	{
	bot->Notice(theClient,"Gline reason can't be more than 255 characters");
	return false;
	}

/*server->setGline( nickUserHost,
	st[ pos ],
	Reason ,
	gLength , bot) ;*/
ccGline *TmpGline = bot->findGline(st[pos]);
bool Up = false;
if(TmpGline)
	Up =  true;	
else TmpGline = new ccGline(bot->SQLDb);
TmpGline->setHost(st [ pos ]);
if(!isPerm)
	TmpGline->setExpires(::time(0) + gLength);
else
	TmpGline->setExpires(0);
TmpGline->setAddedBy(nickUserHost);
TmpGline->setReason(st.assemble( pos + ResStart ));
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

return true ;
}

}
}
