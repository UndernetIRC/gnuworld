/*
 * GLINECommand.cc
 *
 * Glines a specific mask 
 */

#define HUH_NO 3
#define FORCE_NEEDED 2
#define GLINE_OK 1;

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

const char GLINECommand_cc_rcsId[] = "$Id: GLINECommand.cc,v 1.6 2001/05/07 19:02:15 mrbean_ Exp $";

namespace gnuworld
{


using std::string ;

// Input: gline *@blah.net reason
// Input: gline 3600 *@blah.net reason
//
// Output: C GL * +*@lamer.net 3600 :Banned (*@lamer) ...
//
bool GLINECommand::Exec( iClient* theClient, const string& Message )
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

if(bot->CheckGline(st[ pos ].c_str()) == FORCE_NEEDED)
	{
	bot->Notice(theClient,"Please use forcegline to ban that host, for that period of time");
	return false;
	}
else if(bot->CheckGline(st[ pos ].c_str()) == HUH_NO)
	{
	bot->Notice(theClient,"Hmmm naaaaaaaa i dont think so ... ");
	return false;
	}

/*if( hostName.find_first_of( '*' ) != string::npos )
	{
	
	// Wildcard in hostname, do nothing for now.
	}*/

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
	<< ", expires at " << bot->convertToAscTime((time( 0 ) + gLength))
	<< " for: " << st.assemble( pos + 1 )
	<< ends ;
bot->Write( s ) ;
delete[] s.str() ;

return true ;
}
}
