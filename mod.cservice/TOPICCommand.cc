/* 
 * TOPICCommand.cc 
 *
 * 26/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Sets a topic in the channel.
 *
 * Caveats: None
 *
 * $Id: TOPICCommand.cc,v 1.1 2000/12/30 05:47:29 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char TOPICCommand_cc_rcsId[] = "$Id: TOPICCommand.cc,v 1.1 2000/12/30 05:47:29 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool TOPICCommand::Exec( iClient* theClient, const string& Message )
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
