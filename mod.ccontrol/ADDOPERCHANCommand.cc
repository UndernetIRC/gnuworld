/*
 * ADDOPERCHANCommand.cc
 *
 * Add a new irc operator channel 
 *
 */
 
#include	<string>
#include	<cstdlib>
#include        <iomanip>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"

const char ADDOPERCHANCommand_cc_rcsId[] = "$Id: ADDOPERCHANCommand.cc,v 1.8 2002/05/23 17:43:10 dan_karrels Exp $";

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

if(chanName.length() >  channel::MaxName)
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

