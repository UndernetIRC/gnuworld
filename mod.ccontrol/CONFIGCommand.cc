/*
 * CONFIGCommand.cc 
 *
 */

#include	<string>
#include	<cstdlib>
#include	"StringTokenizer.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"

const char CONFIGCommand_cc_rcsId[] = "$Id: CONFIGCommand.cc,v 1.2 2002/05/23 17:43:10 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::ends ;

namespace uworld
{

bool CONFIGCommand::Exec( iClient* theClient, const string& Message)
{
return true ;
}

}

} // namespace gnuworld
