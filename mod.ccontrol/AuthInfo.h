/* AuthInfo.h */

#ifndef __AUTHINFO_H
#define __AUTHINFO_H "$Id: AuthInfo.h,v 1.2 2001/06/11 21:08:31 mrbean_ Exp $"

#include	<string>

#include	<ctime>

namespace gnuworld
{

using std::string ;

class AuthInfo
{

public:
	
	inline const string& 		get_Name() const
		{ return Name; } 

	inline const string&		get_Numeric() const
		{ return Numeric; }
    
	inline const string&		get_SuspendedBy() const
		{ return SuspendedBy; }
	
	inline const unsigned int	get_Id() const
		{ return Id; }
		
	inline const unsigned int	get_Access() const
		{ return Access; }
		
	inline  const unsigned int 	get_Flags() const
		{ return Flags; }
	
	inline const time_t		get_SuspendExpires() const
		{ return SuspendExpires; }
	
	inline void 			set_Name( const string _Name )
		{ Name = _Name; } 			

	inline void 			set_Numeric( const string _Numeric )
		{ Numeric = _Numeric; } 			
	
	inline void 			set_SuspendedBy( const string _SuspendedBy )
		{ SuspendedBy = _SuspendedBy; } 			
	
	inline void 			set_Id( const int _Id )
		{ Id = _Id; } 			

	inline void 			set_Access( const int _Access )
		{ Access = _Access; } 			
	inline void 			set_Flags( const int _Flags )
		{ Flags = _Flags; } 			
	
	inline void 			set_SuspendExpires( const time_t _SuspendExpires )
		{ SuspendExpires = _SuspendExpires; } 			


protected:

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
