/* ccFloodData.h */

#ifndef __CCFLOODDATA_H
#define __CCFLOODDATA_H "$Id: ccFloodData.h,v 1.1 2001/08/16 20:18:38 mrbean_ Exp $"

#include	<string>

#include	<ctime>


namespace gnuworld
{ 

using std::string ;

namespace uworld
{

namespace flood
{
const unsigned int MESSAGE_POINTS = 1;
const unsigned int CTCP_POINTS = 3;	
const unsigned int RESET_TIME = 5;
const unsigned int FLOOD_POINTS = 20;
const int IGNORE_TIME = 15*60;
}

class ccFloodData
{
public:
	
	ccFloodData();
	virtual ~ccFloodData();
	

	inline  unsigned int 	getPoints() const
		{ return Points; }

	inline  time_t		getLastMessage() const
		{ return lastMessage; }
		
	 bool		 	addPoints(unsigned int );
	
protected:
	//Holds the flood points a user has
	unsigned int Points;
	
	//The last time we got data from this user
	time_t lastMessage;

}; //ccFloodData

} //uworld
} //gnuworld	

#endif // __CCFLOODDATA_H