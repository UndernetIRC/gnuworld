/**
 * msg_SQ.cc
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
 * $Id: msg_V.cc,v 1.1 2002/11/08 14:04:44 jeekay Exp $
 */

#include "ServerCommandHandler.h"
#include "xparameters.h"

const char msg_V_cc_rcsId[] = "$Id: msg_V.cc,v 1.1 2002/11/08 14:04:44 jeekay Exp $";

namespace gnuworld
{

CREATE_HANDLER(msg_V)

/**
 * VERSION message handler.
 * AHkav V :Ay
 *
 * AHkav - Requesting user numeric
 * V     - VERSION token (duh)
 * Ay    - Requested server version (should always be us!)
 *
 * As always, the token itself is missing from our xParameters
 */

bool msg_V::Execute( const xParameters& Param )
{

if( Param.size() < 2) {
  elog << "msg_V> Invalid number of parameters received."
       << endl;
  return false;
}



} // bool msg_V::Execute()

} // namespace gnuworld
