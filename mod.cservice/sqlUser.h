/* sqlUser.h */

#ifndef __SQLUSER_H
#define __SQLUSER_H "$Id: sqlUser.h,v 1.13 2001/02/21 00:14:44 dan_karrels Exp $"

#include	<string>
#include	<ctime>
#include	"libpq++.h"
 
namespace gnuworld
{ 
 
using std::string ;

class iClient;

class sqlUser
{

public:

	sqlUser(PgDatabase*) ;
	virtual ~sqlUser() ;

	typedef unsigned short int	flagType ;
	static const flagType F_GLOBAL_SUSPEND =	0x01 ;
	static const flagType F_LOGGEDIN =		0x02 ;
	static const flagType F_INVIS =			0x04 ;

	/*
	 *  Methods to get data atrributes.
	 */ 

	inline const unsigned int&	getID() const
		{ return id ; }

	inline const string&		getUserName() const
		{ return user_name ; }

	inline const string&		getPassword() const
		{ return password ; }

	inline const time_t&		getLastSeen() const
		{ return last_seen ; }

	inline const string&		getEmail() const
		{ return email ; }

	inline const string&		getUrl() const
		{ return url ; }

	inline const unsigned int&	getLanguageId() const
		{ return language_id ; }

	inline const string&		getPublicKey() const
		{ return public_key ; }

	inline bool		getFlag( const flagType& whichFlag ) const
		{ return (whichFlag == (flags & whichFlag)) ; }

	inline const flagType&		getFlags() const
		{ return flags ; }

	inline const string&		getLastUpdatedBy() const
		{ return last_updated_by ; }

	inline const time_t&		getLastUpdated() const
		{ return last_updated ; }

	/*
	 *  Methods to set data atrributes.
	 */

	inline void setFlag( const flagType& whichFlag )
		{ flags |= whichFlag; }

	inline void removeFlag( const flagType& whichFlag )
		{ flags &= ~whichFlag; }

	inline void setPassword( const string& _password )
		{ password = _password; }

	inline void setLastSeen( const time_t& _last_seen )
		{ last_seen = _last_seen; commitLastSeen(); } 

	inline void setLanguageId( const unsigned int& _language_id )
		{ language_id = _language_id; }

	inline iClient* isAuthed()
		{ return networkClient; }

	/*
	 * Method to perform a SQL 'UPDATE' and commit changes to this
	 * object back to the database.
	 */

	bool commit();
	bool commitLastSeen();

	bool loadData( int );
	bool loadData( const string& );
	void setAllMembers(int);
	iClient*	networkClient;
 
protected: 

	unsigned int	id ;
	string		user_name ;
	string		password ;
	time_t		last_seen ;
	string		email ;
	string		url ;
	unsigned int	language_id ;
	string		public_key ;
	flagType	flags ;
	string		last_updated_by ;
	time_t		last_updated ;
 
	PgDatabase*	SQLDb;
} ;

#endif // __SQLUSER_H

} // namespace gnuworld
