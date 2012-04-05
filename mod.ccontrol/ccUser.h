/**
 * ccUser.h
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
 * $Id: ccUser.h,v 1.21 2009/07/26 18:30:38 mrbean_ Exp $
 */

#ifndef __CCUSER_H
#define __CCUSER_H "$Id: ccUser.h,v 1.21 2009/07/26 18:30:38 mrbean_ Exp $"

#include	<string>

#include	<ctime>

#include	"dbHandle.h"

#include	"CControlCommands.h" 

#include	"iClient.h"


#include <vector>
//#include	"ccontrol.h"
namespace gnuworld
{ 

using std::string;
using std::vector;
using std::pair;

namespace uworld
{

namespace operLevel
	{
	const unsigned int UHSLEVEL = 0x01;
	const unsigned int OPERLEVEL = 0x02;
	const unsigned int ADMINLEVEL = 0x03;
	const unsigned int SMTLEVEL = 0x04;
	const unsigned int CODERLEVEL = 0x05;
	const string CODERLEVELSTR = "CODER";
	const string SMTLEVELSTR = "SMT";
	const string ADMINLEVELSTR = "ADMIN";
	const string OPERLEVELSTR = "OPER";
	const string UHSLEVELSTR = "UHS";
	
	
	}
	
/*
CLASS
    ccUser
    
    Holds all the vital information about a user

*/
class ccUser
{
public:
	ccUser(dbHandle*) ;
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
		    
	inline const bool&       	getIsSuspended() const
		    { return IsSuspended ; }
	
	inline const time_t&		getSuspendExpires() const
		    { return SuspendExpires ; }

	inline const time_t&		getPassChangeTS() const
		    { return PassChangeTS; }

	inline const time_t&		getLastAuthTS() const
		    { return LastAuthTS; }

	inline const string&		getLastAuthNumeric() const
		    { return LastAuthNumeric; }

	inline const unsigned int&	getSuspendLevel() const
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
	
	inline bool    		gotFlag(unsigned int _flag) const
		    { return (Flags & _flag ? true : false) ; }

	inline const string&		getServer() const
		    { return Server; }

	inline const bool&		isUhs() const
		    { return IsUhs;  }

	inline const bool&		isOper() const
		    { return IsOper;  }

	inline const bool&		isAdmin() const
		    { return IsAdmin;  }

	inline const bool&		isSmt() const
		    { return IsSmt;  }

	inline const bool&		isCoder() const
		    { return IsCoder;  }

	inline const bool&		getLogs() const
		    { return GetLogs;  }

	inline const bool&		getLag() const
		    { return GetLag;  }

	inline const bool&		getNeedOp() const
		    { return NeedOp;  }

	inline const bool&		getNotice() const
		    { return Notice;  }

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

	inline void			setLastAuthTS(const unsigned int _lastauth)
		    { LastAuthTS = _lastauth; }

	inline void			setPassChangeTS(const unsigned int _PassChangeTS)
		    { PassChangeTS = _PassChangeTS; }

	inline void			setLastAuthNumeric(const string& _numeric)
		    { LastAuthNumeric = _numeric; }

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

	inline void			setLag( const bool _Lag )
		    { GetLag = _Lag; }
		
	inline void			setNeedOp( const bool _needOp )
		    { NeedOp = _needOp; }
	
	inline void			setNotice( const bool _notice )
		    { Notice = _notice; }

	inline void			setClient(iClient* _Client)
		    { Client = _Client; }
		    
	inline void			setSqldb(dbHandle* _SQLDb)
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
	bool GetLag;
	bool NeedOp;
	bool Notice;
	iClient* Client;
	dbHandle* SQLDb;
	time_t LastAuthTS;
	time_t PassChangeTS;
	string LastAuthNumeric;

} ; // class ccUser

}
} // namespace gnuworld

#endif // __CCUSER_H



