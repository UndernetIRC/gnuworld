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

const char JUPECommand_cc_rcsId[] = "$Id: JUPECommand.cc,v 1.11 2001/12/09 14:36:35 mrbean_ Exp $";

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
iServer* Server;
string SName;
unsigned int contime;
if(string::npos != st[ 1 ].find_first_of( '*' ))
	{
	Server = Network->findExpandedServerName(st[1]);
	if(!Server)
		{
		bot->Notice(theClient,"There is no linked server that matches %s"
			    ,st[1].c_str());
			    return false;
		}
	SName = Server->getName();
	contime = Server->getStartTime();
	}
else if(string::npos == st[ 1 ].find_first_of( '.' ))
	{
	bot->Notice( theClient, "Bogus server name" ) ;
	return false ;
	}
else 
	{
	SName = st[1];
	contime = ::time(0);
	}
if(!strcasecmp(SName,Network->findServer(bot->getUplink()->getUplinkCharYY())->getName()))
	{
	bot->Notice(theClient,"What are you trying to do? get me splited?");
	bot->MsgChanLog("%s just tried to jupe my uplink!\n",theClient->getNickName().c_str());
	return false;
	}
bot->MsgChanLog("%s is asking me to jupe %s because : %s\n",theClient->getNickName().c_str(),SName.c_str(),st.assemble(2).c_str());

if(dbConnected)
	{
	ccServer* tmpServer = new ccServer(bot->SQLDb);
	if(tmpServer->loadData(SName))
		{
		tmpServer->setSplitReason(string("Squited by ") + theClient->getNickUserHost() 
					    + string(" because : ") + st.assemble(2));
		tmpServer->setLastSplitted(time(0));
		tmpServer->Update();
		}				    
	delete tmpServer;
	}
		
// This will squit the server, if it exists or not, or if it is
// already juped.
//server->SquitServer( st[ 1 ], "Prepare to be juped" ) ;
/*strstream s;
s 	<< bot->getCharYYXXX()
	<< " SQUIT " 
	<< SName
	<< " " << contime
	<< " :"
	<< st.assemble(2)
	<< ends;
bot->Write(s);
delete[] s.str();*/

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
