/*
 * CONFIGCommand.cc 
 *
 */

#include	<string>
#include	<strstream>
#include	<cstdlib>
#include	"StringTokenizer.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"

const char CONFIGCommand_cc_rcsId[] = "$Id: CONFIGCommand.cc,v 1.1 2002/03/01 18:27:36 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::strstream ;
using std::ends ;

namespace uworld
{

bool CONFIGCommand::Exec( iClient* theClient, const string& Message)
{

return true ;
}

}

} // namespace gnuworld
