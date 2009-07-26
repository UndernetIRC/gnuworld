/**
 * ccFlood.h
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
 * $Id: ccFloodData.h,v 1.6 2009/07/26 18:30:38 mrbean_ Exp $
 */

#ifndef __CCFLOODDATA_H
#define __CCFLOODDATA_H "$Id: ccFloodData.h,v 1.6 2009/07/26 18:30:38 mrbean_ Exp $"

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

	inline const int& 	getLogins() const
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
