/* ccFlood.h */

#ifndef __CCFLOODDATA_H
#define __CCFLOODDATA_H "$Id: ccFloodData.h,v 1.4 2001/12/13 08:50:00 mrbean_ Exp $"

#define IGNORE_NOT_FOUND -1
#define IGNORE_REMOVED -2

#include	<string>

#include	<ctime>

namespace gnuworld
{ 

using std::string ;

namespace uworld
{

class ccFloodData
{
public:
	ccFloodData(const string &);

	virtual ~ccFloodData();
	//Mehods for getting info
	
	inline const string 	getNumeric() const
		{ return Numeric; }

	inline const int 	getLogins() const
		{ return Logins; }

	inline time_t	 	getIgnoreExpires() const
		{ return IgnoreExpires; }
	inline const string	getIgnoredHost() const
		{ return IgnoredHost; }
	
	//Methods for setting info
	
	inline void 		setNumeric( string _Numeric ) 
		{ Numeric = _Numeric; }

	inline void 		setLogins( int _Logins ) 
		{ Logins = _Logins; }

	inline void		add_Login()
		{ Logins++; }

	inline 	void		resetIgnore()
		{ IgnoreExpires = 0; IgnoredHost="" ; }
	
	inline void		resetLogins() 
		{ Logins = 0; }
	
	inline void		setIgnoreExpires(time_t _Expires)
		{ IgnoreExpires =_Expires; }

	inline void		setIgnoredHost(string _Host)
		{ IgnoredHost =_Host; }
	
	static unsigned int numAllocated;
	
       inline  unsigned int    getPoints() const
                { return Points; }

        inline  time_t          getLastMessage() const
                { return lastMessage; }

         bool                   addPoints(unsigned int );
 
protected:
	string Numeric;
	
	int Logins;
	
	time_t IgnoreExpires;

	string IgnoredHost;

       //Holds the flood points a user has
        unsigned int Points;
        
        //The last time we got data from this user
        time_t lastMessage;
}; // class ccFloodData
}
} // namespace gnuworld

#endif // __CCFLOODDATA_H
