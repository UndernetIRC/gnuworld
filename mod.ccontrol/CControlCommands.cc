/* CControlCommands.cc
 */

#include	<string>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"iClient.h"

namespace gnuworld
{

using std::string ;

namespace uworld
{

void Command::Usage( iClient* theClient )
{
bot->Notice( theClient, string( "Usage: " ) + ' ' + getInfo() ) ;
}

}
} // namespace gnuworld
