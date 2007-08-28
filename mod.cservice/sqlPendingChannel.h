/**
 * sqlPendingChannel.h
 *
 * Stores information about channels currently being registered.
 * We do a number of things to channels in this state ;)
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
 * $Id: sqlPendingChannel.h,v 1.8 2007/08/28 16:10:12 dan_karrels Exp $
 */

#ifndef __SQLPENDINGCHANNEL_H
#define __SQLPENDINGCHANNEL_H "$Id: sqlPendingChannel.h,v 1.8 2007/08/28 16:10:12 dan_karrels Exp $"

#include	<map>

#include	"sqlPendingTraffic.h"
#include	"dbHandle.h"

namespace gnuworld
{ 
 
class sqlPendingChannel
{
public:
	sqlPendingChannel(dbHandle*);
	~sqlPendingChannel();

	bool commit();
	bool commitSupporter(unsigned int, unsigned int);
	void loadTrafficCache();

	unsigned int channel_id;
	unsigned int join_count;
	unsigned int unique_join_count;

	typedef std::map < int, int > supporterListType;
	supporterListType supporterList;

	typedef std::map < unsigned int, sqlPendingTraffic* > trafficListType;
	trafficListType trafficList;

	dbHandle*	SQLDb;
};

}
#endif // __SQLPENDINGCHANNEL_H
