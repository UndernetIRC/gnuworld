/**
 * clientExample.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: clientExample.cc,v 1.5 2003/06/28 16:26:45 dan_karrels Exp $
 */

#include	"client.h"
#include	"clientExample.h"
#include	"server.h"
#include	"EConfig.h"

namespace gnuworld
{

/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */

extern "C"
{
  xClient* _gnuwinit(const string& args)
  { 
    return new clientExample( args );
  }

} 
 
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

void clientExample::OnPrivateMessage( iClient* theClient,
	const string&, bool )
{
Notice( theClient, "Howdy :)" ) ;
}

// Burst any channels.
bool clientExample::BurstChannels()
{
Join( "#some_oper_channel" ) ;
return xClient::BurstChannels() ;
}

} // namespace gnuworld
