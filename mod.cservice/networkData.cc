/**
 * networkData.cc
 * 
 * This class is used for storing/modifying "Runtime" specific data
 * applicable to a active iClient on the network.
 *
 * N.B: ENSURE this class is free'd on EVENT QUIT/KILL for the iClient
 * is it relating too or it'll leak. :)
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
 * $Id: networkData.cc,v 1.9 2005/12/27 13:27:59 kewlio Exp $
 */
 
#include	<sstream>
#include	<string> 

#include	<cstring> 

#include	"ELog.h"
#include	"misc.h"
#include	"networkData.h" 

const char networkData_h_rcsId[] = __NETWORKDATA_H ;
const char networkData_cc_rcsId[] = "$Id: networkData.cc,v 1.9 2005/12/27 13:27:59 kewlio Exp $" ;

namespace gnuworld
{

using std::string ;
using std::endl ;
 
networkData::networkData()
 : messageTime( 0 ),
   outputCount( 0 ),
   flood_points( 0 ),
   currentUser( 0 ), 
   ignored( false ),
   ipr_ts( 0 ),
   failed_logins( 0 )
{
}
 
networkData::~networkData()
{}
 
} // Namespace gnuworld.
