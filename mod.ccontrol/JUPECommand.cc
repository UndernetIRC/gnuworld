/*
 * JUPECommand.cc
 *
 * Jupitur a  server
 */

#include	<new>
#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"iServer.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"Constants.h"

const char JUPECommand_cc_rcsId[] = "$Id: JUPECommand.cc,v 1.14 2002/03/01 18:27:36 mrbean_ Exp $";

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
if( st[1].length() > server::MaxName) 
	{
	bot->Notice( theClient, "Bogus server name" ) ;
	return false ;
	}
bot->MsgChanLog("JUPE %s\n",st.assemble(1).c_str());

iServer* Server;
string SName;
unsigned int contime;
if(string::npos != st[ 1 ].find_first_of( '*' ))
	{
	bot->Notice(theClient,"Sorry, but you must give a full server name when juping!");
	return false;
	}	
else if(string::npos == st[ 1 ].find_first_of( '.' ))
	{
	bot->Notice( theClient, "Bogus server name" ) ;
	return false ;
	}

Server = Network->findServerName(st[1]);
if(Server)
	{
	bot->Notice(theClient,"%s is currently linked to the network"
		    ", please squit it first"
		    ,st[1].c_str());
		    return false;
	}
SName = st[1];
contime = ::time(0);

if(!strcasecmp(SName,Network->findServer(bot->getUplink()->getUplinkCharYY())->getName()))
	{
	bot->Notice(theClient,"What are you trying to do? get me splited?");
	bot->MsgChanLog("%s just tried to jupe my uplink!\n",theClient->getNickName().c_str());
	return false;
	}
bot->MsgChanLog("%s is asking me to jupe %s because : %s\n",theClient->getNickName().c_str(),SName.c_str(),st.assemble(2).c_str());


iServer* jupeServer = new (std::nothrow) iServer(
	0, // uplinkIntYY
	"", // charYYXXX
	SName,
	time( 0 ) ) ;
assert( jupeServer != 0 ) ;

// Attach the new (fake) server.
server->AttachServer( jupeServer, st.assemble( 2 ) ) ;

return true ;

}

}
} // namespace gnuworld
