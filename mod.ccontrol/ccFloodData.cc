/* 
 * ccFloodData.cc
 * 
 * $Id: ccFloodData.cc,v 1.1 2001/08/16 20:18:38 mrbean_ Exp $
 */
 
#include	<strstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"ELog.h"
#include	"misc.h"

#include	"ccFloodData.h" 

const char ccFloodData_h_rcsId[] = __CCFLOODDATA_H ;
const char ccFLOODDATA_cc_rcsId[] = "$Id: ccFloodData.cc,v 1.1 2001/08/16 20:18:38 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::strstream ;
using std::ends ;

namespace uworld
{

ccFloodData::ccFloodData()
 : Points(0),
   lastMessage(0)
{
}

ccFloodData::~ccFloodData()
{
}

bool ccFloodData::addPoints(unsigned int _Points)
{
//Check if the flood points needs to be reset
if(flood::RESET_TIME < (::time(0) - lastMessage))
	Points = 0;

//Update the new flood points
Points += _Points;
lastMessage = ::time(0);

//We have updated everything, check if the user needs to be silenced

return (Points > flood::FLOOD_POINTS);
}

} // uworld
} // gnuworld

      

