
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
