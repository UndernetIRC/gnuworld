/* clientExample.cc */

#include	"client.h"
#include	"clientExample.h"
#include	"server.h"
#include	"EConfig.h"

namespace gnuworld
{

/**
 * This constructor calls the base class constructor.  The xClient
 * constructor will open the configuration file given and retrieve
 * basic client info (nick/user/host/etc).
 * Any additional processing must be done here.
 */
clientExample::clientExample( const string& configFileName )
 : xClient( configFileName )
{}

clientExample::~clientExample()
{
/* No heap space allocated */
}

int clientExample::OnPrivateMessage( iClient* theClient, const string& message )
{
Notice( theClient, "Howdy :)" ) ;
return 0 ;
}

// Burst any channels.
int clientExample::BurstChannels()
{
Join( "#some_oper_channel" ) ;
return 0 ;
}

} // namespace gnuworld
