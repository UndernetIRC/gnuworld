/* sqlUser.h */

#ifndef __SQLUSER_H
#define __SQLUSER_H "$Id: sqlUser.h,v 1.32 2003/01/08 23:23:37 gte Exp $"

#include	<string>
#include	<vector>
#include	<ctime>
#include	"libpq++.h"

namespace gnuworld
{

using std::string ;
using std::vector ;

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
	static const flagType F_NONOTES;

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

	inline const time_t&		getInstantiatedTS() const
		{ return instantiated_ts ; }

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

	inline const unsigned int& getMaxLogins() const
		{ return maxlogins ; }

	inline const time_t& getLastNote() const
		{ return last_note ; }

	inline const unsigned int& getNotesSent() const
		{ return notes_sent ; }

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

	inline void setLastSeen( const time_t& _last_seen )
		{ last_seen = _last_seen; commitLastSeenWithoutMask(); }

	inline void setLanguageId( const unsigned int& _language_id )
		{ language_id = _language_id; }

	inline void setLastUsed( const time_t& _last_used )
		{ last_used = _last_used; }

	inline void setInstantiatedTS( const time_t& _instantiated_ts)
		{ instantiated_ts = _instantiated_ts; }

	inline void setEmail( const string& _email )
		{ email = _email; }

	inline void setMaxLogins( const unsigned int& _maxlogins )
		{ maxlogins = _maxlogins; }

	inline void setLastNote( const time_t& _last_note )
		{ last_note = _last_note; }

	inline void setNotesSent( const unsigned int& _notes_sent )
		{ notes_sent = _notes_sent; }

	/*
	 * Method to perform a SQL 'UPDATE' and commit changes to this
	 * object back to the database.
	 */

	bool commit(iClient* who);
	bool commitLastSeen();
	bool commitLastSeenWithoutMask();
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
	time_t		instantiated_ts;
	string		email ;
	string		last_hostmask ;
	unsigned int maxlogins;
	time_t		last_note;
	unsigned int	notes_sent;

	PgDatabase*	SQLDb;
} ;

} // namespace gnuworld

#endif // __SQLUSER_H

