/*
 * LISTOPERCHANSCommand.cc
 *
 * List the channels thare are marked as ircops only
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char LISTOPERCHANSCommand_cc_rcsId[] = "$Id: LISTOPERCHANSCommand.cc,v 1.6 2002/05/23 17:43:11 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// listoperchans
bool LISTOPERCHANSCommand::Exec( iClient* theClient, const string& Message )
{

bot->Notice( theClient, "There are currently %d IRCoperator only channels",
	bot->operChan_size() ) ;

if( bot->operChan_empty() )
	{
	return true ;
	}

string chanList = "" ;
ccontrol::const_operChanIterator ptr = bot->operChan_begin() ;

while( ptr != bot->operChan_end() )
	{
	if( !chanList.empty() )
		{
		chanList += ", " ;
		chanList += *ptr ;
		}
	else
		{
		chanList = *ptr ;
		}
	++ptr ;
	}

bot->Notice( theClient, chanList ) ;
return true ;
}
}
}
