/* CControlCommands.cc
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
//#include	<netinet/in.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"misc.h"
#include	"Gline.h"
#include	"ip.h"
#include	"ELog.h"
#include	"md5hash.h" 

namespace gnuworld
{

//namespace ccontrolns
//{

using std::string ;

void Command::Usage( iClient* theClient )
{
bot->Notice( theClient, string( "Usage: " ) + ' ' + getInfo() ) ;
}

//} // close namespace ccontrolns

} // namespace gnuworld
