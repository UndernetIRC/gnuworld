/* ccFlood.h */

#ifndef __CCLOGIN_H
#define __CCLOGIN_H "$Id: ccLogin.h,v 1.4 2001/06/07 15:59:06 mrbean_ Exp $"

#define IGNORE_NOT_FOUND -1
#define IGNORE_REMOVED -2

#include	<string>

#include	<ctime>

#include	"libpq++.h"

namespace gnuworld
{ 

using std::string ;

class ccLogin
{
public:
	ccLogin(const string &);

	virtual ~ccLogin();
	//Mehods for getting info
	
	inline const string 	get_Numeric() const
		{ return Numeric; }

	inline const int 	get_Logins() const
		{ return Logins; }

	inline time_t	 	get_IgnoreExpires() const
		{ return IgnoreExpires; }
	inline const string	get_IgnoredHost() const
		{ return IgnoredHost; }
	
	//Methods for setting info
	
	inline void 		set_Numeric( string _Numeric ) 
		{ Numeric = _Numeric; }

	inline void 		set_Logins( int _Logins ) 
		{ Logins = _Logins; }

	inline void		add_Login()
		{ Logins++; }

	inline 	void		resetIgnore()
		{ IgnoreExpires = 0; IgnoredHost="" ; }
	
	inline void		resetLogins() 
		{ Logins = 0; }
	
	inline void		set_IgnoreExpires(time_t _Expires)
		{ IgnoreExpires =_Expires; }

	inline void		set_IgnoredHost(string _Host)
		{ IgnoredHost =_Host; }
	

	
protected:
	string Numeric;
	int Logins;
	time_t IgnoreExpires;
	string IgnoredHost;
}; // class ccGline

} // namespace gnuworld

#endif // __CCGLINE_H
