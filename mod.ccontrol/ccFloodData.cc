/* 
 * ccFloodData.cc
 * 
 * flood data class
 * 
 * $Id: ccFloodData.cc,v 1.5 2002/05/23 17:43:11 dan_karrels Exp $
 */
 
#include	<string> 
#include	<iostream>

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"ccFloodData.h" 
#include	"Constants.h"

const char ccFloodData_h_rcsId[] = __CCFLOODDATA_H ;
const char ccFloodData_cc_rcsId[] = "$Id: ccFloodData.cc,v 1.5 2002/05/23 17:43:11 dan_karrels Exp $" ;

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
