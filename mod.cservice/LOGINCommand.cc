/* LOGINCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 

const char LOGINCommand_cc_rcsId[] = "$Id: LOGINCommand.cc,v 1.1 2000/12/11 02:04:28 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool LOGINCommand::Exec( iClient* theClient, const string& Message )
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
