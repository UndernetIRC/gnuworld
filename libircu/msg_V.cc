/**
 * msg_V.cc
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
 * $Id: msg_V.cc,v 1.7 2005/03/25 03:07:29 dan_karrels Exp $
 */

#include	<sstream>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"ServerCommandHandler.h"
#include	"xparameters.h"

RCSTAG( "$Id: msg_V.cc,v 1.7 2005/03/25 03:07:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::endl ;
using std::stringstream ;

CREATE_HANDLER(msg_V)

/**
 * VERSION message handler:
 *  AHkav V :Ay
 *
 * AHkav - Requesting user numeric
 * V     - VERSION token (duh)
 * Ay    - Requested server version (should always be us!)
 *
 * As always, the token itself is missing from our xParameters
 */

bool msg_V::Execute( const xParameters& Param )
{
/* We should have exactly two parameters - source and destination */
if( Param.size() != 2)
	{
	elog	<< "msg_V> Invalid number of parameters received."
		<< endl;
	return false;
	}

/* The destination numeric should always match us exactly */
if(strncmp(Param[1], theServer->getCharYY().c_str(), 2) != 0)
	{
	elog	<< "msg_V> Target server is not me!"
		<< endl;
	return false;
	}

/* 
 * Should we check if the client exists here?
 * If the version request got to us, presumably it has to exist
 * on the network, even if we don't know about it.
 */

/*
 * Reply looks like:
 *  A8 351 AyAAA u2.10.11.01. devlink.netgamers.org :B27AeEFfIKMpSU
 *
 * A8    - Answering server
 * 351   - Numeric for version reply
 * AyAAA - Target (requester)
 * 'the rest' - Reply.
 */
stringstream versionReply;
versionReply << theServer->getCharYY()
             << " 351 "
             << Param[0]
             << " :" __DATE__ " " __TIME__
             << " GNUworld Services Core" ;

theServer->Write(versionReply);

return true;

} // bool msg_V::Execute()

} // namespace gnuworld
