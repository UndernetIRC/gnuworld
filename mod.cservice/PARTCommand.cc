/* PARTCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 

const char PARTCommand_cc_rcsId[] = "$Id: PARTCommand.cc,v 1.1 2001/01/29 02:16:27 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool PARTCommand::Exec( iClient* theClient, const string& Message )
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
