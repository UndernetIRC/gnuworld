/* sqlUser.h */

#ifndef __SQLUSER_H
#define __SQLUSER_H "$Id: sqlUser.h,v 1.5 2001/01/02 07:55:12 gte Exp $"

#include	<string>
#include	<ctime>
#include	"libpq++.h"

using std::string ;

namespace gnuworld
{ 
 
class sqlUser
{

public:

	sqlUser(PgDatabase*) ;
	virtual ~sqlUser() ;

	typedef unsigned short int	flagType ;
	static const flagType F_GLOBAL_SUSPEND =	0x01 ;

	const unsigned int&	getID() const
		{ return id ; }
	const string&		getUserName() const
		{ return user_name ; }
	const string&		getPassword() const
		{ return password ; }
	const time_t&		getLastSeen() const
		{ return last_seen ; }
	const string&		getEmail() const
		{ return email ; }
	const string&		getUrl() const
		{ return url ; }
	const unsigned int&	getLanguageId() const
		{ return language_id ; }
	const string&		getPublicKey() const
		{ return public_key ; }
	inline bool		getFlag( const flagType& whichFlag ) const
		{ return (flags & whichFlag) ; }
	const flagType&		getFlags() const
		{ return flags ; }
	const string&		getLastUpdateBy() const
		{ return last_update_by ; }
	const time_t&		getLastUpdate() const
		{ return last_update ; }

	bool loadData( int );
	bool loadData( const string& );

protected:

    void setAllMembers(int);

	unsigned int	id ;
	string		user_name ;
	string		password ;
	time_t		last_seen ;
	string		email ;
	string		url ;
	unsigned int	language_id ;
	string		public_key ;
	flagType	flags ;
	string		last_update_by ;
	time_t		last_update ;
 
	PgDatabase*	SQLDb;
} ;

#endif // __SQLUSER_H

} // Namespace gnuworld
