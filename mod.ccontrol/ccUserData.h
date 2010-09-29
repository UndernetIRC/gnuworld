/**
 * ccUserData.h
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
 * $Id: ccUserData.h,v 1.2 2003/06/28 01:21:19 dan_karrels Exp $
 */

#ifndef __CCUSERDATA_H_
#define __CCUSERDATA_H_

#include "iClient.h"
#include "ccUser.h"
#include "ccFloodData.h"

namespace gnuworld
{

namespace uworld
{

class ccUserData
{

public:
	
	ccUserData(ccFloodData* tFlood = NULL,ccUser* tDbUser = NULL) 
	: flood(tFlood),
	dbUser(tDbUser) 
	{}
	
	virtual ~ccUserData(){}
	
	inline ccUser* getDbUser() const
		{ return dbUser; }
		
	inline ccFloodData* getFlood() const
		{ return flood; }
	
	inline void setDbUser(ccUser* _dbUser) 
		{ dbUser = _dbUser; }
	
	inline void setFlood(ccFloodData* _flood) 
		{ flood = _flood; }
protected:

	
	ccFloodData* flood;	

	ccUser* dbUser;


};

}
}

#endif
