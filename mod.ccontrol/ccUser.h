/* ccUser.h */

#ifndef __CCUSER_H
#define __CCUSER_H "$Id: ccUser.h,v 1.5 2001/02/25 19:52:06 mrbean_ Exp $"

#include	<string>

#include	<ctime>

#include	"libpq++.h"
 
namespace gnuworld
{ 

using std::string ;

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
        
	inline const time_t&	getSuspendExpires() const
		{ return SuspendExpires ; }

	inline const string&		getSuspendedBy() const
		{ return SuspendedBy ; }

	inline const unsigned int&	getAccess() const
		{ return Access ; }

	inline const unsigned int&	getFlags() const
		{ return Flags ; }

	/*
	 * Methods to set data attributes
	 */

	inline void setID( const unsigned int _id )
		{ Id = _id; }
	 
	inline void setUserName( const string& _username )
		{ UserName = _username; }

	inline void setPassword( const string& _password )
		{ Password = _password; }

	inline void setLast_Updated_By( const string& _last_updated_by )
		{ last_updated_by = _last_updated_by; }

	inline void setNumeric( const string& _numeric )
		{ Numeric = _numeric; }

	inline void setSuspendExpires( const unsigned int _expire )
		{ SuspendExpires = _expire; }

	inline void setSuspendedBy( const string& _suspendedby )
		{ SuspendedBy = _suspendedby; }

	inline void removeCommand( const unsigned int _command )
		{ Access &= ~_command; }

	inline void setAccess( const unsigned int _access )
		{ Access = _access; }

	inline void addCommand( const unsigned int _command )
		{ Access |= _command; }

	inline void setFlags( const unsigned int _flags )
		{ Flags = _flags; }

	inline void removeFlag( const unsigned int _flag )
		{ Flags &= ~_flag; }

	inline void setFlag( const unsigned int _flag )
		{ Flags |= _flag; }

	/*
	 * Methods to load a user and update the 
	 * the database
	 */

	bool loadData( const string& Name );
	
	bool loadData( const unsigned int Id );

	bool Update();

	void GetParm();
	
protected:
	unsigned int Id;
	string UserName;
	string Password;
	string last_updated_by;
	string Numeric;
	time_t SuspendExpires;
	string SuspendedBy;
	unsigned int Access;
	unsigned int Flags;
	PgDatabase* SQLDb;

} ; // class ccUser

} // namespace gnuworld

#endif // __CCUSER_H



