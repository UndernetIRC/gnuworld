/* AuthInfo.h */

#ifndef __AUTHINFO_H
#define __AUTHINFO_H "$Id: AuthInfo.h,v 1.1 2001/03/02 02:02:00 dan_karrels Exp $"

#include	<string>

#include	<ctime>

namespace gnuworld
{

using std::string ;

class AuthInfo
{

public:



//protected:

	string		Name;
	string		Numeric;
	string		SuspendedBy;

	unsigned int	Id;
	unsigned int	Access;
	unsigned int	Flags;

	time_t		SuspendExpires;

} ;

} // namespace gnuworld

#endif // __AUTHINFO_H
