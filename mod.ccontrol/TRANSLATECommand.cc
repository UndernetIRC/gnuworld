/*
 * TRANSLATECommand.cc
 *
 * Translates a numeric to a nick
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>
#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char TRANSLATECommand_cc_rcsId[] = "$Id: TRANSLATECommand.cc,v 1.10 2002/05/23 17:43:11 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// translate yyxxx
bool TRANSLATECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() != 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

iClient* Target = Network->findClient( st[ 1 ] ) ;

if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find numeric %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}
bot->MsgChanLog("TRANSLATE %s\n",st.assemble(1).c_str());

iServer* theServer = Network->findServer( Target->getIntYY() ) ;
if( NULL == theServer )
	{
	elog	<< "TRANSLATECommand> Unable to find server\n" ;
	return false ;
	}

bot->Notice( theClient, "%s is %s!%s@%s on server %s",
	st[ 1 ].c_str(),
	Target->getNickName().c_str(),
	Target->getUserName().c_str(),
	Target->getInsecureHost().c_str(),
	theServer->getName().c_str() ) ;

return true ;

}
}
}
