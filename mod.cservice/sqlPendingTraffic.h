/**
 * sqlPendingTraffic.h
 *
 * Stores information about the uniqueness of visitors to
 * pending channels.
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
 * $Id: sqlPendingTraffic.h,v 1.4 2007/08/28 16:10:12 dan_karrels Exp $
 */

#ifndef __SQLPENDINGTRAFFIC_H
#define __SQLPENDINGTRAFFIC_H "$Id: sqlPendingTraffic.h,v 1.4 2007/08/28 16:10:12 dan_karrels Exp $"

#include	"dbHandle.h"
 
namespace gnuworld
{ 
 
class sqlPendingTraffic
{

public:
	sqlPendingTraffic(dbHandle*);
	bool insertRecord();
	bool commit();

	unsigned int channel_id; 
	unsigned int ip_number;
	unsigned int join_count; 

	dbHandle*	SQLDb;
};

}
#endif // __SQLPENDINGTRAFFIC_H 
