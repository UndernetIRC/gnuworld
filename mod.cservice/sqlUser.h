/**
 * sqlUser.h
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
 * $Id: sqlUser.h,v 1.42 2008/11/12 20:45:42 mrbean_ Exp $
 */

#ifndef __SQLUSER_H
#define __SQLUSER_H "$Id: sqlUser.h,v 1.42 2008/11/12 20:45:42 mrbean_ Exp $"

#include	<string>
#include	<vector>

#include	<ctime>

#include	"cservice_config.h"
#include	"dbHandle.h"

namespace gnuworld
{

class iClient;

class sqlUser
{
public:
	sqlUser(dbHandle*) ;
	virtual ~sqlUser() ;

	typedef unsigned short int	flagType ;
	static const flagType F_GLOBAL_SUSPEND;
	static const flagType F_LOGGEDIN; // Deprecated.
	static const flagType F_INVIS;
	static const flagType F_FRAUD;
	static const flagType F_NONOTES;
	static const flagType F_NOPURGE;
	static const flagType F_NOADMIN;
	static const flagType F_ALUMNI;
	static const flagType F_OPER;
	static const flagType F_NOADDUSER;

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

	inline const std::string&	getUserName() const
		{ return user_name ; }

	inline const std::string&	getPassword() const
		{ return password ; }

	inline const std::string&	getUrl() const
		{ return url ; }

	inline const unsigned int&	getLanguageId() const
		{ return language_id ; }

	inline bool		getFlag( const flagType& whichFlag ) const
		{ return (whichFlag == (flags & whichFlag)) ; }

	inline const flagType&		getFlags() const
		{ return flags ; }

	inline const std::string&	getLastUpdatedBy() const
		{ return last_updated_by ; }

	inline const time_t&		getLastUpdated() const
		{ return last_updated ; }

	inline const time_t&		getLastUsed() const
		{ return last_used ; }

	inline const time_t&		getInstantiatedTS() const
		{ return instantiated_ts ; }

	inline const std::string&	getEmail() const
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

	inline const unsigned int& getFailedLogins() const
		{ return failed_logins ; }

	inline const unsigned int& getLastFailedLoginTS() const
		{ return failed_login_ts ; }

	/*
	 *  Methods to set data atrributes.
	 */

	inline void setFlag( const flagType& whichFlag )
		{ flags |= whichFlag; }

	inline void removeFlag( const flagType& whichFlag )
		{ flags &= ~whichFlag; }

	inline void setPassword( const std::string& _password )
		{ password = _password; }

	inline void setLastSeen( const time_t& _last_seen,
			const std::string& _last_hostmask,
			const std::string& _last_ip)
		{ last_seen = _last_seen;
		  last_hostmask = _last_hostmask ;
		  last_ip = _last_ip;
		  commitLastSeen(); }

	inline void setLastSeen( const time_t& _last_seen )
		{ last_seen = _last_seen; commitLastSeenWithoutMask(); }

	inline void setLanguageId( const unsigned int& _language_id )
		{ language_id = _language_id; }

	inline void setLastUsed( const time_t& _last_used )
		{ last_used = _last_used; }

	inline void setInstantiatedTS( const time_t& _instantiated_ts)
		{ instantiated_ts = _instantiated_ts; }

	inline void setEmail( const std::string& _email )
		{ email = _email; }

	inline void setMaxLogins( const unsigned int& _maxlogins )
		{ maxlogins = _maxlogins; }

	inline void setLastNote( const time_t& _last_note )
		{ last_note = _last_note; }

	inline void setNotesSent( const unsigned int& _notes_sent )
		{ notes_sent = _notes_sent; }

	inline void setUserName( const std::string& _user_name )
		{ user_name = _user_name ; }

	inline void setLastUpdatedBy( const std::string& _last_updated_by )
		{ last_updated_by = _last_updated_by ; }

	inline void incFailedLogins()
		{ failed_logins++; }

	inline void setFailedLogins( const unsigned int& _failed_logins )
		{ failed_logins = _failed_logins ; }

	inline void setLastFailedLoginTS( const unsigned int& _ts )
		{ failed_login_ts = _ts ; }

	/*
	 * Method to perform a SQL 'UPDATE' and commit changes to this
	 * object back to the database.
	 */

	bool commit(iClient* who);
	bool commitLastSeen();
	bool commitLastSeenWithoutMask();
	time_t	getLastSeen();
	const std::string getLastHostMask();
	const std::string getLastIP();
	bool Insert() ;

	bool loadData( int );
	bool loadData( const std::string& );
	void setAllMembers( int );
	void writeEvent( unsigned short, sqlUser*, const std::string& );
	const std::string getLastEvent( unsigned short, unsigned int&);

	/*
	 * List of all network users authenticated as this account.
	 */
	typedef std::vector <iClient*>	networkClientListType;
	networkClientListType networkClientList;

protected:

	unsigned int	id ;
	std::string	user_name ;
	std::string	password ;
	time_t		last_seen ;
	std::string	url ;
	unsigned int	language_id ;
	flagType	flags ;
	std::string	last_updated_by ;
	time_t		last_updated ;
	time_t		last_used;
	time_t		instantiated_ts;
	std::string	email ;
	std::string	last_hostmask ;
	std::string last_ip;
	unsigned int	maxlogins;
	time_t		last_note;
	unsigned int	notes_sent;
	unsigned int	failed_logins;
	unsigned int	failed_login_ts;

	dbHandle*	SQLDb;
} ;

} // namespace gnuworld

#endif // __SQLUSER_H

