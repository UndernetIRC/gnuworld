/* JOINCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 

const char JOINCommand_cc_rcsId[] = "$Id: JOINCommand.cc,v 1.1 2001/01/29 02:16:27 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool JOINCommand::Exec( iClient* theClient, const string& Message )
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
