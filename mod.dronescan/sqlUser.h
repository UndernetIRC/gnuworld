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
 */

#ifndef SQLUSER_H
#define SQLUSER_H "$Id: sqlUser.h,v 1.4 2007/08/28 16:10:14 dan_karrels Exp $"

#include <string>

#include	"dbHandle.h"

namespace gnuworld {

namespace ds {

using std::string;

class sqlUser {
public:
	sqlUser(dbHandle*);
	virtual ~sqlUser();

	typedef unsigned int flagType;
	static const flagType F_SUSPENDED;

	/* Accessors */

	inline const string& getUserName() const
		{ return user_name; }

	inline const int getCreated() const
		{ return created; }

	inline const unsigned int getLastSeen() const
		{ return last_seen; }

	inline const string& getLastUpdatedBy() const
		{ return last_updated_by; }

	inline const unsigned int getLastUpdated() const
		{ return last_updated; }

	inline const unsigned int getAccess() const
		{ return access; }

	/* Mutators */

	inline void setUserName(const string& _user_name)
		{ user_name = _user_name; }

	inline void setCreated(const unsigned int _created)
		{ created = _created; }

	inline void setLastSeen(const unsigned int _last_seen)
		{ last_seen = _last_seen; }

	inline void setLastUpdatedBy(const string& _last_updated_by)
		{ last_updated_by = _last_updated_by; }

	inline void setLastUpdated(const unsigned int _last_updated)
		{ last_updated = _last_updated; }

	inline void setAccess(const unsigned int _access)
		{ access = _access; }

	inline void setFlags(const flagType _flags)
		{ flags = _flags; }

	/* Convenience functions for flags */
	inline const bool isSuspended() const
		{ return (flags & F_SUSPENDED); }


	/* Methods to alter our SQL status */
	void setAllMembers(int);
	bool commit();
	bool insert();
	bool remove();

protected:
	string		user_name;
	unsigned int	created;
	unsigned int	last_seen;
	string		last_updated_by;
	unsigned int	last_updated;
	unsigned int	flags;
	unsigned int	access;

	dbHandle*	SQLDb;
}; // class sqlUser

} // namespace ds

} // namespace gnuworld

#endif
