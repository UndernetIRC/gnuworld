/*
 * FORCECommand.cc
 *
 * Glines a specific mask 
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

const char FORCEGLINECommand_cc_rcsId[] = "$Id: FORCEGLINECommand.cc,v 1.1 2001/05/08 16:01:12 mrbean_ Exp $";

namespace gnuworld
{


using std::string ;

// Input: forcegline *@blah.net reason
// Input: forcegline 3600 *@blah.net reason
//
// Output: C GL * +*@lamer.net 3600 :Banned (*@lamer) ...
//
bool FORCEGLINECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

StringTokenizer::size_type pos = 1 ;

time_t gLength = bot->getDefaultGlineLength() ;
if( atoi( st[ pos ].c_str() ) > 0 )
	{
	// User has specified a gline length
	if( st.size() < 4 )
		{
		Usage( theClient ) ;
		return true ;
		}

	gLength = atoi( st[ pos ].c_str() ) ;
	pos++ ;
	}

// (pos) is the index of the next token, the user@host mask.

string::size_type atPos = st[ pos ].find_first_of( '@' ) ;
if( string::npos == atPos )
	{
	// User has only specified hostname, not a user name
	bot->Notice( theClient, "GLINE: Please specify gline mask in the "
		"format: user@host" ) ;
	return true ;
	}

string userName = st[ pos ].substr( 0, pos ) ;
string hostName = st[ pos ].substr( pos + 1 ) ;

switch(bot->CheckGline(st[ pos ].c_str(),gLength))
	{
	case FORCE_NEEDED_HOST:
	    bot->MsgChanLog("%s is using forcegline to gline %s\n",theClient->getNickName().c_str(),st[pos].c_str());
	    break;
	case FORCE_NEEDED_TIME:
	    bot->MsgChanLog("%s is using forcegline to force a 2+ days gline\n",theClient->getNickName().c_str());
	    break;
	case FORCE_NEEDED_USERS:
	    bot->MsgChanLog("%s is using forcegline to gline more than 32 users\n",theClient->getNickName().c_str());
	    break;
	case HUH_NO_HOST:
	    bot->Notice(theClient,"I dont think glining *@* is such a good idea, do you?");
	    return false;
	case HUH_NO_USERS:
	    bot->Notice(theClient,"Glining more than 256 ppl is a NoNo");
	    return false;
	}	


// Avoid passing a reference to a temporary variable.
string nickUserHost = theClient->getNickUserHost() ;

server->setGline( nickUserHost,
	st[ pos ],
	st.assemble( pos + 1 ),
	gLength ) ;

strstream s ;
s	<< server->getCharYY() << " WA :"
        << theClient->getNickName().c_str()
	<< " is adding gline for: "
	<< st[ pos ]
	<< ", expires at " << (time( 0 ) + gLength)
	<< " reason: " << st.assemble( pos + 1 )
	<< ends ;
bot->Write( s ) ;
delete[] s.str() ;

return true ;
}
}
