/*
 * REMOVEOPERCHANCommand.cc
 *
 * Removes an ircop only channel
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char REMOPERCHANCommand_cc_rcsId[] = "$Id: REMOPERCHANCommand.cc,v 1.6 2001/07/30 16:58:39 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

// remoperchan #channel

namespace uworld
{

bool REMOPERCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[1].size() > 200)
	{
	bot->Notice(theClient,"Channel name can't be more than 200 chars");
	return false;
	}
string chanName = st[ 1 ] ;
if( '#' != chanName[ 0 ] )
	{
	bot->Notice( theClient, "Invalid channel name" ) ;
	return true ;
	}

if( bot->removeOperChan( chanName ) )
	{
	bot->Notice( theClient, "Removal of %s as oper chan SUCCEEDED",
		chanName.c_str() ) ;
	}
else
	{
	bot->Notice( theClient, "Removal of %s as oper chan FAILED",
		chanName.c_str() ) ;
	}
return true ;
}

}
}
