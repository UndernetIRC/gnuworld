/*
 * sqlPendingChannel Class Header.
 * Stores information about channels currently being registered.
 * We do a number of things to channels in this state ;)
 */

#ifndef __SQLPENDINGCHANNEL_H
#define __SQLPENDINGCHANNEL_H "$Id: sqlPendingChannel.h,v 1.3 2001/04/30 01:26:21 gte Exp $"

#include	<string> 
 
using std::string ;

namespace gnuworld
{ 
 
class sqlPendingChannel
{

public:
	sqlPendingChannel();
	bool commit();

	unsigned int channel_id;
	unsigned int join_count;
	typedef map < int, int > supporterListType;
	supporterListType supporterList;
};

}
#endif // __SQLPENDINGCHANNEL_H
