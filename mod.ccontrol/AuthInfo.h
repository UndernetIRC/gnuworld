/* AuthInfo.h */

#ifndef __AUTHINFO_H
#define __AUTHINFO_H "$Id: AuthInfo.h,v 1.6 2001/07/29 13:33:20 mrbean_ Exp $"

#include	<string>

#include	<ctime>

#include 	<CControlCommands.h>

namespace gnuworld
{

using std::string ;

namespace uworld
{

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
		
	inline const unsigned long int	getAccess() const
		{ return Access; }

	inline const unsigned long int	getSAccess() const
		{ return SAccess; }
		
	inline  const unsigned int 	getFlags() const
		{ return Flags; }
	
	inline const bool		getIsSuspended() const
		{ return IsSuspended; }
		
	inline const time_t		getSuspendExpires() const
		{ return SuspendExpires; }
	
	inline const string&		getServer() const
		{ return Server; }
		
	inline const bool		getNeedOp() const
		{ return NeedOp; }

	inline const bool		getLogs() const
		{ return GetLogs; }
		
	inline void 			setName( const string _Name )
		{ Name = _Name; } 			

	inline void 			setNumeric( const string _Numeric )
		{ Numeric = _Numeric; } 			
	

	inline void 			setId( const int _Id )
		{ Id = _Id; } 			

	inline void 			setAccess( const unsigned long int _Access )
		{ Access = _Access; } 			

	inline void 			setSAccess( const unsigned long int _SAccess )
		{ SAccess = _SAccess; } 			

	inline void 			setFlags( const int _Flags )
		{ Flags = _Flags; } 			
	
	inline void			setIsSuspended( const bool _suspend )
		{ IsSuspended = _suspend; }
			
	inline void 			setSuspendExpires( const time_t _SuspendExpires )
		{ SuspendExpires = _SuspendExpires; } 			
	
	inline void 			setSuspendedBy( const string _SuspendedBy )
		{ SuspendedBy = _SuspendedBy; } 			
	
	
	inline void 			setServer( const string& _Server )
		{ Server = _Server; }

	inline void			setNeedOp( const bool _NeedOp )
		{ NeedOp = _NeedOp; }
	
	inline void			setGetLogs( const bool _GetLogs )
		{ GetLogs = _GetLogs; }

	bool gotAccess(Command* Comm);
		
protected:

	string		Name;
	string		Numeric;
	bool		IsSuspended;
	string		SuspendedBy;
	time_t		SuspendExpires;
	string 		Server;
	unsigned int	Id;
	unsigned long int Access;
	unsigned long int SAccess;
	unsigned int	Flags;
	bool		NeedOp;
	bool		GetLogs;

} ;

}

} // namespace gnuworld

#endif // __AUTHINFO_H
