/*
 * GLINECommand.cc
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

const char GLINECommand_cc_rcsId[] = "$Id: GLINECommand.cc,v 1.10 2001/05/21 16:14:55 mrbean_ Exp $";

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

switch(bot->CheckGline(st[ pos ].c_str(),gLength))
	{
	case FORCE_NEEDED_HOST:
	    bot->Notice(theClient,"Please use forcegline to gline that host");
	    return false;
	case FORCE_NEEDED_TIME:
	    bot->Notice(theClient,"Please use forcegline to gline for that amount of time");
	    return false;
	case FORCE_NEEDED_USERS:
	    bot->Notice(theClient,"You gline bans for than 32 users , please use forcegline");
	    return false;
	case HUH_NO_HOST:
	    bot->Notice(theClient,"I dont think glining that host is such a good idea, do you?");
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

ccGline *TmpGline = bot->findGline(st[pos]);
bool Up = false;

if(TmpGline)
	Up =  true;	
else TmpGline = new ccGline(bot->SQLDb);
TmpGline->set_Host(st [ pos ]);
TmpGline->set_Expires(::time(0) + gLength);
TmpGline->set_AddedBy(nickUserHost);
TmpGline->set_Reason(st.assemble( pos +1 ));
TmpGline->set_AddedOn(::time(0));
if(Up)
	{	
	TmpGline->Update();
	bot->wallopsAsServer("%s is refreshing Gline expiration time on host %s for %d\n",
	theClient->getNickName().c_str(),st[pos].c_str(),::time(0) + gLength);
	}
else
	{
	TmpGline->Insert();
	//We need to update the Id
	TmpGline->loadData(TmpGline->get_Host());
	bot->wallopsAsServer("%s is adding gline for %s, expires at %s for: %s\n"
	,theClient->getNickName().c_str(),
	st[pos].c_str(),bot->convertToAscTime(time( 0 ) + gLength),
	st.assemble( pos + 1 ).c_str());
	}
if(!Up)
	bot->addGline(TmpGline);
return true ;
}
}
