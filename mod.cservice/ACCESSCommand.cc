/* ACCESSCommand.cc */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 

const char ACCESSCommand_cc_rcsId[] = "$Id: ACCESSCommand.cc,v 1.2 2000/12/11 02:04:28 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool ACCESSCommand::Exec( iClient* theClient, const string& Message )
{
/*
 * access::= 'access' [ LWS access_option ] LWS #channel [ LWS access_option ] 
 *                    [(userid|nick|hostmask)] [ LWS access_option ]
 * access_option::= '-modif' [ LWS access_option ] 
 *                 | '-min' LWS access [ LWS access_option ]
 *                 | '-max' LWS access [ LWS access_option ]
 *                 | '-' [ 'no' ] 'autoop' [ LWS access_option ]
 *                 | '-modif' [mask] [ LWS access_option ] 
 */

	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}
 
	return true ;
} 

} // namespace gnuworld.
