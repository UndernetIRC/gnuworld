/* ccFlood.h */

#ifndef __CCLOGIN_H
#define __CCLOGIN_H "$Id: ccLogin.h,v 1.7 2001/11/21 20:54:40 mrbean_ Exp $"

#define IGNORE_NOT_FOUND -1
#define IGNORE_REMOVED -2

#include	<string>

#include	<ctime>

#include	"libpq++.h"

namespace gnuworld
{ 

using std::string ;

namespace uworld
{

class ccLogin
{
public:
	ccLogin(const string &);

	virtual ~ccLogin();
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
	
protected:
	string Numeric;
	int Logins;
	time_t IgnoreExpires;
	string IgnoredHost;
}; // class ccGline
}
} // namespace gnuworld

#endif // __CCGLINE_H
