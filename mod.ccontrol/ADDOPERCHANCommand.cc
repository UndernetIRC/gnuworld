/*
 * ADDOPERCHANCommand.cc
 *
 * Add a new irc operator channel 
 *
 */
 
#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char ADDOPERCHANCommand_cc_rcsId[] = "$Id: ADDOPERCHANCommand.cc,v 1.6 2001/07/23 10:28:51 mrbean_ Exp $";

namespace gnuworld
{

namespace uworld
{

using std::string ;

bool ADDOPERCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

string chanName = st[ 1 ] ;
if( '#' != chanName[ 0 ] )
	{
	bot->Notice( theClient, "Invalid channel name" ) ;
	return true ;
	}

if(chanName.length() >  200)
	{
	bot->Notice( theClient,"Channel name too long" );
	return true;
	}

if( bot->addOperChan( chanName ) )
	{
	bot->Notice( theClient, "Addition of %s as oper chan SUCCEEDED",
		chanName.c_str() ) ;
	}
else
	{
	bot->Notice( theClient, "Addition of %s as oper chan FAILED",
		chanName.c_str() ) ;
	}
return true ;
}
}
}

