/**
 * ccFloodData.cc
 * flood data class
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
 * $Id: ccFloodData.cc,v 1.8 2005/01/12 03:50:29 dan_karrels Exp $
 */
 
#include	<string> 
#include	<iostream>

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"ccFloodData.h" 
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: ccFloodData.cc,v 1.8 2005/01/12 03:50:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 

namespace uworld
{

unsigned int ccFloodData::numAllocated = 0;

ccFloodData::ccFloodData(const string &_Numeric)
 : Numeric(_Numeric),
   Logins(0),
   IgnoreExpires( 0 ),
   Points(0),
   lastMessage(0)

{
++numAllocated;
}

ccFloodData::~ccFloodData()
{
--numAllocated;
}

bool ccFloodData::addPoints(unsigned int _Points)
{
//Check if the flood points needs to be reset
if((signed)flood::RESET_TIME <   (::time(0) - lastMessage))
        Points = 0;   

//Update the new flood points
Points += _Points;
lastMessage = ::time(0);

//We have updated everything, check if the user needs to be silenced

return (Points > flood::FLOOD_POINTS);
}
 
}
} //Namespace Gnuworld
