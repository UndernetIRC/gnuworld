/*
 * GCHANCommand.cc
 *
 * close down a channel
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
//#include	"gline.h"
#include 	"time.h"
#include	"Constants.h"

const char GCHANCommand_cc_rcsId[] = "$Id: GCHANCommand.cc,v 1.5 2001/12/08 17:17:29 mrbean_ Exp $";

namespace gnuworld
{


using std::string ;


namespace uworld
{

bool GCHANCommand::Exec( iClient* theClient, const string& Message )
{
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

// (pos) is the index of the next token, the user@host mask.

if( (st[pos].substr(0,1) != "#" ) || (st[pos].size() > channel::MaxName))
	{
	// Channel name must start with #
	bot->Notice( theClient, "GCHAN: Please specify a legal channel name "
		"must start with # and no longer than %d chars",channel::MaxName ) ;
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
	bot->Notice(theClient,"No duration was set, setting to %d seconds by default",gLength) ;
	ResStart = 1;
		}
	}
else if(st.size() < 4)
	{
	Usage(theClient);
	return false;
	}

// Avoid passing a reference to a temporary variable.
string nickUserHost = theClient->getNickUserHost() ;
string Reason = st.assemble( pos + ResStart );
if(Reason.size() > 255)
	{
	bot->Notice(theClient,"Gline reason can't be more than 255 chars");
	return false;
	}

server->setGline( nickUserHost,
	st[ pos ],
	Reason ,
	gLength ) ;

ccGline *TmpGline = bot->findGline(bot->removeSqlChars(st[pos]));
bool Up = false;
if(TmpGline)
	Up =  true;	
else TmpGline = new ccGline(bot->SQLDb);
TmpGline->setHost(bot->removeSqlChars(st [ pos ]));
if(!isPerm)
	TmpGline->setExpires(::time(0) + gLength);
else
	TmpGline->setExpires(0);
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

return true ;
}

}
}
