/* 
 * ccLogin.cc
 * 
 * Login class
 * 
 * $Id: ccLogin.cc,v 1.3 2001/11/21 20:54:40 mrbean_ Exp $
 */
 
#include	<strstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"ccLogin.h" 

const char ccLogin_h_rcsId[] = __CCLOGIN_H ;
const char ccLogin_cc_rcsId[] = "$Id: ccLogin.cc,v 1.3 2001/11/21 20:54:40 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::strstream ;
using std::ends ;

namespace uworld
{

unsigned int ccLogin::numAllocated = 0;

ccLogin::ccLogin(const string &_Numeric)
 : Numeric(_Numeric),
   Logins(0),
   IgnoreExpires( 0 )
{
++numAllocated;
}

ccLogin::~ccLogin()
{
--numAllocated;
}

}
} //Namespace Gnuworld
