/* sqlUser.cc */

#include	<string>

#include	<ctime>

#include	"sqlUser.h"

using std::string ;

sqlUser::sqlUser()
 : id( 0 ),
   last_seen( 0 ),
   language_id( 0 ),
   flags( 0 ),
   last_update( ::time( 0 ) )
{
}

sqlUser::~sqlUser()
{}


