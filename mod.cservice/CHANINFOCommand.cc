/* 
 * CHANINFOCommand.cc 
 *
 * 29/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Displays information about a channel.
 *
 * Caveats: None
 *
 * $Id: CHANINFOCommand.cc,v 1.2 2000/12/30 05:47:29 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"levels.h"
#include	"responses.h"
 
const char CHANINFOCommand_cc_rcsId[] = "$Id: CHANINFOCommand.cc,v 1.2 2000/12/30 05:47:29 gte Exp $" ;

//[04:21] -W- #coder-com is registered by:
//[04:21] -W- Special last seen: 103 days, 01:01:58 ago
//[04:21] -W- Isoper last seen: 12 days, 17:03:57 ago
//[04:21] -W- Desc: Required: 2.10.07.1.(12)  Recommended: 2.10.10.pl12  Development: 2.10.10.alpha
//[04:21] -W- URL: http://coder-com.undernet.org

namespace gnuworld
{

using namespace gnuworld;
 
bool CHANINFOCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}
 
	return true ;
} 

} // namespace gnuworld.
