/* AuthInfo.h */

#ifndef __AUTHINFO_H
#define __AUTHINFO_H "$Id: AuthInfo.h,v 1.3 2001/07/20 09:09:31 mrbean_ Exp $"

#include	<string>

#include	<ctime>

namespace gnuworld
{

using std::string ;

class AuthInfo
{

public:
	
	inline const string& 		getName() const
		{ return Name; } 

	inline const string&		getNumeric() const
		{ return Numeric; }
    
	inline const string&		getSuspendedBy() const
		{ return SuspendedBy; }
	
	inline const unsigned int	getId() const
		{ return Id; }
		
	inline const unsigned int	getAccess() const
		{ return Access; }
		
	inline  const unsigned int 	getFlags() const
		{ return Flags; }
	
	inline const time_t		getSuspendExpires() const
		{ return SuspendExpires; }
	
	inline const string&		getServer() const
		{ return Server; }
		
	inline void 			setName( const string _Name )
		{ Name = _Name; } 			

	inline void 			setNumeric( const string _Numeric )
		{ Numeric = _Numeric; } 			
	
	inline void 			setSuspendedBy( const string _SuspendedBy )
		{ SuspendedBy = _SuspendedBy; } 			
	
	inline void 			setId( const int _Id )
		{ Id = _Id; } 			

	inline void 			setAccess( const int _Access )
		{ Access = _Access; } 			
	inline void 			setFlags( const int _Flags )
		{ Flags = _Flags; } 			
	
	inline void 			setSuspendExpires( const time_t _SuspendExpires )
		{ SuspendExpires = _SuspendExpires; } 			
	
	inline void 			setServer( const string& _Server )
		{ Server = _Server; }

protected:

	string		Name;
	string		Numeric;
	string		SuspendedBy;
	string 		Server;
	unsigned int	Id;
	unsigned int	Access;
	unsigned int	Flags;

	time_t		SuspendExpires;

} ;

} // namespace gnuworld

#endif // __AUTHINFO_H
