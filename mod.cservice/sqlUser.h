/* sqlUser.h */

#ifndef __SQLUSER_H
#define __SQLUSER_H "$Id: sqlUser.h,v 1.26 2002/01/08 23:20:43 gte Exp $"

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
	static const flagType F_GLOBAL_SUSPEND;
	static const flagType F_LOGGEDIN; // Deprecated.
	static const flagType F_INVIS;
	static const flagType F_FRAUD;

	/*
	 *   User 'Event' Flags, used in the userlog table.
	 */

	static const unsigned int	EV_SUSPEND;
	static const unsigned int	EV_UNSUSPEND;
	static const unsigned int	EV_ADMINMOD;
	static const unsigned int	EV_MISC;
	static const unsigned int	EV_COMMENT;

	/*
	 *  Methods to get data atrributes.
	 */

	inline const unsigned int&	getID() const
		{ return id ; }

	inline const string&		getUserName() const
		{ return user_name ; }

	inline const string&		getPassword() const
		{ return password ; }

	inline const string&		getUrl() const
		{ return url ; }

	inline const unsigned int&	getLanguageId() const
		{ return language_id ; }

	inline bool		getFlag( const flagType& whichFlag ) const
		{ return (whichFlag == (flags & whichFlag)) ; }

	inline const flagType&		getFlags() const
		{ return flags ; }

	inline const string&		getLastUpdatedBy() const
		{ return last_updated_by ; }

	inline const time_t&		getLastUpdated() const
		{ return last_updated ; }

	inline const time_t&		getLastUsed() const
		{ return last_used ; }

	inline const string&		getEmail() const
		{ return email ; }

	inline bool isAuthed()
		{ return (networkClientList.size() != 0); }

	inline void addAuthedClient(iClient* theClient)
		{ networkClientList.push_back(theClient); } ;

	inline void removeAuthedClient(iClient* theClient)
		{
		networkClientListType::iterator ptr = networkClientList.begin();
		while( ptr != networkClientList.end() )
			{
				iClient* testClient = *ptr;
				if (testClient == theClient)
					{
					ptr = networkClientList.erase(ptr);
					} else
					{
						++ptr;
					}
			}
		} ;

	/*
	 *  Methods to set data atrributes.
	 */

	inline void setFlag( const flagType& whichFlag )
		{ flags |= whichFlag; }

	inline void removeFlag( const flagType& whichFlag )
		{ flags &= ~whichFlag; }

	inline void setPassword( const string& _password )
		{ password = _password; }

	inline void setLastSeen( const time_t& _last_seen, const string& _last_hostmask )
		{ last_seen = _last_seen; last_hostmask = _last_hostmask ; commitLastSeen(); }

	inline void setLanguageId( const unsigned int& _language_id )
		{ language_id = _language_id; }

	inline void setLastUsed( const time_t& _last_used )
		{ last_used = _last_used; }

	inline void setEmail( const string& _email )
		{ email = _email; }

	/*
	 * Method to perform a SQL 'UPDATE' and commit changes to this
	 * object back to the database.
	 */

	bool commit();
	bool commitLastSeen();
	time_t	getLastSeen();
	const string getLastHostMask();

	bool loadData( int );
	bool loadData( const string& );
	void setAllMembers( int );
	void writeEvent( unsigned short, sqlUser*, const string& );
	const string getLastEvent( unsigned short, unsigned int&);

	/*
	 * List of all network users authenticated as this account.
	 */
	typedef vector <iClient*>	networkClientListType;
	networkClientListType networkClientList;

protected:

	unsigned int	id ;
	string		user_name ;
	string		password ;
	time_t		last_seen ;
	string		url ;
	unsigned int	language_id ;
	flagType	flags ;
	string		last_updated_by ;
	time_t		last_updated ;
	time_t		last_used;
	string		email ;
	string		last_hostmask ;

	PgDatabase*	SQLDb;
} ;

} // namespace gnuworld

#endif // __SQLUSER_H

