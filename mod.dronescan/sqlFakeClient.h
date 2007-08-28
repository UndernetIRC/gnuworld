/**
 * sqlFakeClient.h
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
 */

#ifndef SQLFAKECLIENT_H
#define SQLFAKECLIENT_H "$Id: sqlFakeClient.h,v 1.4 2007/08/28 16:10:14 dan_karrels Exp $"

#include <string>

#include <ctime>

#include	"dbHandle.h"

namespace gnuworld {

namespace ds {

using std::string ;

class sqlFakeClient {
public:
	sqlFakeClient(dbHandle*);
	virtual ~sqlFakeClient();

	typedef unsigned int flagType;
	static const flagType F_ACTIVE;

	/* Accessors */
	inline unsigned int getId() const
		{ return id; }

	inline string getNickName() const
		{ return nickname; }

	inline string getUserName() const
		{ return username; }

	inline string getHostName() const
		{ return hostname; }

	inline string getRealName() const
		{ return realname; }

	inline string getCreatedBy() const
		{ return createdBy_s; }

	inline time_t getCreatedOn() const
		{ return createdOn; }

	inline time_t getLastUpdated() const
		{ return lastUpdated; }

	inline bool getFlag(flagType _flag) const
		{ return (flags & _flag); }

	/* Mutators */
	inline void setNickName(const string& _nickname)
		{ nickname = _nickname; }

	inline void setUserName(const string& _username)
		{ username = _username; }

	inline void setHostName(const string& _hostname)
		{ hostname = _hostname; }

	inline void setRealName(const string& _realname)
		{ realname = _realname; }

	inline void setCreatedBy(int _createdby)
		{ createdBy_i = _createdby; }

	inline void setFlag(flagType _flag)
		{ flags |= _flag; }

	inline void removeFlag(flagType _flag)
		{ flags &= !_flag; }

	/* Convenience functions for flags */
	string getFlagsString() const;

	inline bool isActive() const
		{ return (flags & F_ACTIVE); }

	/* Core */
	void setAllMembers(int);
	bool commit();
	bool insert();
	bool remove();

	/* Misc helper functions */
	string getNickUserHost() const;

protected:
	unsigned int	id;
	string		nickname;
	string		username;
	string		hostname;
	string		realname;
	unsigned int	createdBy_i;
	string		createdBy_s;
	time_t		createdOn;
	time_t		lastUpdated;

	unsigned int	flags;

	dbHandle *SQLDb;
}; // class sqlFakeClient

} // namespace ds

} // namespace gnuworld

#endif
