#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

namespace gnuworld
{

using std::string ;

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
