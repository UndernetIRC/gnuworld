/*
 * JUPECommand.cc
 *
 * Jupitur a  server
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"iServer.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char JUPECommand_cc_rcsId[] = "$Id: JUPECommand.cc,v 1.9 2001/07/23 10:28:51 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool JUPECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return false ;
	}

// The server name to be juped must have at least 1 '.'
if( (string::npos == st[ 1 ].find_first_of( '.' )) || (st[1].length() > 63) )
	{
	bot->Notice( theClient, "Bogus server name" ) ;
	return false ;
	}

bot->MsgChanLog("%s is asking me to jupe %s because : %s\n",theClient->getCharYYXXX().c_str(),st[1].c_str(),st.assemble(2).c_str());

// This will squit the server, if it exists or not, or if it is
// already juped.
//server->SquitServer( st[ 1 ], "Prepare to be juped" ) ;

iServer* jupeServer = new (nothrow) iServer(
	0, // uplinkIntYY
	"", // charYYXXX
	st[ 1 ],
	time( 0 ) ) ;
assert( jupeServer != 0 ) ;

// Attach the new (fake) server.
server->AttachServer( jupeServer, st.assemble( 2 ) ) ;

return true ;

}

}
} // namespace gnuworld
