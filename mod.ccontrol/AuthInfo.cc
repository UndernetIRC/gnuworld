/* 
 * AuthInfo.cc
 * 
 * Storage class for authetication information
 * 
 * $Id: AuthInfo.cc,v 1.1 2001/07/29 13:33:20 mrbean_ Exp $
 */
 
#include	<strstream>
#include	<string> 

#include	<cstring> 

#include	"libpq++.h"
#include	"ELog.h"
#include	"misc.h"
#include	"AuthInfo.h" 

const char AuthInfo_h_rcsId[] = __AUTHINFO_H ;
const char ccAuthInfo_cc_rcsId[] = "$Id: AuthInfo.cc,v 1.1 2001/07/29 13:33:20 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::strstream ;
using std::ends ;

namespace uworld
{
bool AuthInfo::gotAccess(Command* Comm)
{
if(Comm->getSecondAccess())
	{
	return ((SAccess & Comm->getFlags()) != 0 ? true : false);
	}
else
	{
	return ((Access & Comm->getFlags()) != 0 ? true : false);	
	}
}

}
}
