/* 
 * networkData.cc
 * 
 * This class is used for storing/modifying "Runtime" specific data applicable to a
 * active iClient on the network.
 *
 * N.B: ENSURE this class is free'd on EVENT QUIT/KILL for the iClient is it relating too,
 * or it'll leak. :)
 *
 */
 
#include	<strstream>
#include	<string> 
#include	<cstring> 
#include	"ELog.h"
#include	"misc.h"
#include	"networkData.h" 

using std::string ;
using std::endl ;

namespace gnuworld
{

using namespace gnuworld ;
 
networkData::networkData()
 : lastMessage( 0 ),
   outputCount( 0 ),
   inputCount( 0 ),
   currentUser( 0 ),
   ignored( false) 
{
}
 
networkData::~networkData()
{}
 
} // Namespace gnuworld.
