/* RANDOMCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 

const char RANDOMCommand_cc_rcsId[] = "$Id: RANDOMCommand.cc,v 1.2 2001/02/21 00:14:43 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
 
bool RANDOMCommand::Exec( iClient* theClient, const string& Message )
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
