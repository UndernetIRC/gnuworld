/* ccUser.h */

#ifndef __CCUSER_H
#define __CCUSER_H "$Id: ccUser.h,v 1.14 2001/12/13 08:50:00 mrbean_ Exp $"

#include	<string>

#include	<ctime>

#include	"libpq++.h"

#include	"CControlCommands.h" 

#include	"iClient.h"
//#include	"ccontrol.h"
namespace gnuworld
{ 

using std::string ;

namespace uworld
{

namespace operLevel
	{
	const unsigned int UHSLEVEL = 0x01;
	const unsigned int OPERLEVEL = 0x02;
	const unsigned int ADMINLEVEL = 0x03;
	const unsigned int SMTLEVEL = 0x04;
	const unsigned int CODERLEVEL = 0x05;
	}
	
class ccUser
{

public:

	ccUser(PgDatabase*) ;
	virtual ~ccUser() ;

	/*
	 *  Methods to get data atrributes.
	 */ 

	inline const unsigned int&	getID() const
		    { return Id ; }

	inline const string&		getUserName() const
		    { return UserName ; }

	inline const string&		getPassword() const
		    { return Password ; }

	inline const string&		getLast_Updated_by() const
		    { return last_updated_by ; }

	inline const string&		getNumeric() const
		    { return Numeric ; }

        inline const string&		getEmail() const
		    { return Email ; }
		    
	inline const bool       	getIsSuspended() const
		    { return IsSuspended ; }
	
	inline const time_t&		getSuspendExpires() const
		    { return SuspendExpires ; }

	inline const unsigned int	getSuspendLevel() const
		{ return SuspendLevel; }
		
	inline const string&		getSuspendedBy() const
		    { return SuspendedBy ; }

	inline const string&		getSuspendReason() const
		    { return SuspendReason ; }
	
	inline const unsigned long int&	getAccess() const
		    { return Access ; }

	inline const unsigned long int&	getSAccess() const
		    { return SAccess ; }

	inline const unsigned int&	getFlags() const
		    { return Flags ; }
	
	inline const bool    		gotFlag(unsigned int _flag) const
		    { return (Flags & _flag ? true : false) ; }

	inline const string&		getServer() const
		    { return Server; }

	inline const bool		isUhs() const
		    { return IsUhs;  }

	inline const bool		isOper() const
		    { return IsOper;  }

	inline const bool		isAdmin() const
		    { return IsAdmin;  }

	inline const bool		isSmt() const
		    { return IsSmt;  }

	inline const bool		isCoder() const
		    { return IsCoder;  }

	inline const bool		getLogs() const
		    { return GetLogs;  }

	inline const bool		getNeedOp() const
		    { return NeedOp;  }

	inline const iClient*		getClient() const
		    { return Client;  }

	/*
	 * Methods to set data attributes
	 */

	inline void 			setID( const unsigned int _id )
		    { Id = _id; }
	 
	inline void 			setUserName( const string& _username )
		    { UserName = _username; }

	inline void 			setPassword( const string& _password )
		    { Password = _password; }

	inline void 			setLast_Updated_By( const string& _last_updated_by )
		    { last_updated_by = _last_updated_by; }

	inline void 			setNumeric( const string& _numeric )
		    { Numeric = _numeric; }

	inline void 			setEmail(const string& _Email)
		    { Email = _Email; }
		    
	inline void 			setIsSuspended(const bool _suspeneded)
		    { IsSuspended = _suspeneded; }
		
	inline void 			setSuspendExpires( const unsigned int _expire )
		    { SuspendExpires = _expire; }

	inline void 			setSuspendLevel( const unsigned int _level )
		    { SuspendLevel = _level; }
		    
	inline void 			setSuspendedBy( const string& _suspendedby )
		    { SuspendedBy = _suspendedby; }

	inline void 			setSuspendReason( const string& _reason )
		    { SuspendReason = _reason; }

	inline void 			setAccess( const unsigned long int _access )
		    { Access = _access; }

	inline void 			setSAccess( const unsigned long int _saccess )
		    { SAccess = _saccess; }


	inline void 			setFlags( const unsigned int _flags )
		    { Flags = _flags; }

	inline void 			removeFlag( const unsigned int _flag )
		    { Flags &= ~_flag; }

	inline void 			setFlag( const unsigned int _flag )
		    { Flags |= _flag; }

	inline void 			setServer( const string& _server )
		    { Server = _server; }
	
	inline void			setLogs( const bool _Logs )
		    { GetLogs = _Logs; }
	
	inline void			setNeedOp( const bool _needOp )
		    { NeedOp = _needOp; }
	
	inline void			setClient(iClient* _Client)
		    { Client = _Client; }
		    
	inline void			setSqldb(PgDatabase* _SQLDb)
		    { SQLDb = _SQLDb; }
	/*
	 * Methods to load a user and update the 
	 * the database
	 */

	bool loadData( const string& Name );
	
	bool loadData( const unsigned int Id );

    	bool Update();

	void GetParm();
	
	void setUhs();

	void setOper();

	void setAdmin();

	void setSmt();

	void setCoder();
	
	unsigned int getType();
	
	void setType(unsigned int Type);
	
	bool gotAccess(Command* Comm);
	
	void addCommand(Command* Comm);
	
	void removeCommand(Command* Comm);

	void updateAccessFromFlags();

	void updateAccess(unsigned int Type);
	
	static unsigned int numAllocated;
	
protected:
	unsigned int Id;
	string UserName;
	string Password;
	string last_updated_by;
	string Numeric;
	string Server;
	string Email;
	bool   IsSuspended;
	time_t SuspendExpires;
	string SuspendedBy;
	unsigned int SuspendLevel;
	string SuspendReason;
	unsigned long int Access;
	unsigned long int SAccess;
	unsigned int Flags;
	bool IsUhs;
	bool IsOper;
	bool IsAdmin;
	bool IsSmt;
	bool IsCoder;
	bool GetLogs;
	bool NeedOp;
	iClient* Client;
	PgDatabase* SQLDb;

} ; // class ccUser

}
} // namespace gnuworld

#endif // __CCUSER_H



