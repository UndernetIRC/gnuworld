/* 
 * sqlPendingChannel.cc
 * 
 * Class which contains details about channels which are 'pending'
 * registration.
 * 
 * $Id: sqlPendingChannel.cc,v 1.1 2001/04/30 01:26:21 gte Exp $
 */
 
#include	<strstream>
#include	<string> 

#include	<cstring> 
#include	<ctime>

#include	"ELog.h"
#include	"misc.h"
#include	"sqlLevel.h"
#include	"sqlUser.h"
#include	"sqlChannel.h"
#include	"constants.h"
#include	"cservice_config.h"
#include	"sqlPendingChannel.h"
 
const char sqlPendingChannel_h_rcsId[] = __SQLPENDINGCHANNEL_H ;
const char sqlPendingChannel_cc_rcsId[] = "$Id: sqlPendingChannel.cc,v 1.1 2001/04/30 01:26:21 gte Exp $" ;

namespace gnuworld
{
using std::string ; 
using std::endl ; 

sqlPendingChannel::sqlPendingChannel()
:channel_id(0),
join_count(0)
{ 
}

bool sqlPendingChannel::commit()
{
	elog << "Commiting Pending Channel Details:"
	<< "Channel ID: " << channel_id
	<< ", Total Join Count: " << join_count
	<< endl;

	return true;
}


}
