
#ifndef __CCUSER_H
#define __CCUSER_H "$Id: ccUser.h,v 1.2 2001/02/24 19:22:08 mrbean_ Exp $"

#include	<string>
#include	<ctime>
#include	"libpq++.h"
 


namespace gnuworld
{ 

using std::string ;

class iClient;
 
class ccUser
{

public:

	ccUser(PgDatabase*) ;
	virtual ~ccUser() ;

	/*
	 *  Methods to get data atrributes.
	 */ 

	const unsigned int&	getID() const
		{ return Id ; }

	const string&		getUserName() const
		{ return UserName ; }

	const string&		getPassword() const
		{ return Password ; }

	const string&		getLast_Updated_by() const
		{ return last_updated_by ; }

	const string&		getNumeric() const
		{ return Numeric ; }
        
	const unsigned int&	getSuspendExpires() const
		{ return SuspendExpires ; }

	const string&		getSuspendedBy() const
		{ return SuspendedBy ; }

	const unsigned int&	getAccess() const
		{ return Access ; }

	const unsigned int&	getFlags() const
		{ return Flags ; }

	/*
	 * Methods to set data attributes
	 */
	 
	inline void setUserName( const string& _username )
		{ UserName = _username; }

	inline void setPassword( const string& _password )
		{ Password = _password; }

	inline void setLast_Updated_By( const string& _last_updated_by )
		{ last_updated_by = _last_updated_by; }

	inline void setNumeric( const string& _numeric )
		{ Numeric = _numeric; }

	inline void setSuspendExpires( const unsigned _expire )
		{ SuspendExpires = _expire; }

	inline void setSuspendedBy( const string& _suspendedby )
		{ SuspendedBy = _suspendedby; }

	inline void removeCommand( const unsigned int _command )
		{ Access &= ~_command; }

	inline void addCommand( const unsigned int _command )
		{ Access |= _command; }

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
        unsigned int SuspendExpires;
        string SuspendedBy;
        unsigned int Access;
        unsigned int Flags;
	PgDatabase* SQLDb;

} ; 


}
#endif // __CCUSER_H



