#ifndef SQLUSER_H
#define SQLUSER_H "$Id: sqlUser.h,v 1.1 2003/07/26 16:47:18 jeekay Exp $"

#include <string>

class PgDatabase;

namespace gnuworld {

namespace ds {

using std::string;

class sqlUser {
public:
	sqlUser(PgDatabase*);
	virtual ~sqlUser();
	
	typedef unsigned int flagType;
	static const flagType F_SUSPENDED;
	
	/* Accessors */
	
	inline const string& getUserName() const
		{ return user_name; }
	
	inline const unsigned int getLastSeen() const
		{ return last_seen; }
	
	inline const string& getLastUpdatedBy() const
		{ return last_updated_by; }
	
	inline const unsigned int getLastUpdated() const
		{ return last_updated; }
	
	inline const unsigned int getAccess() const
		{ return access; }
	
	/* Mutators */
	
	inline void setLastSeen(const unsigned int _last_seen)
		{ last_seen = _last_seen; }
	
	inline void setLastUpdatedBy(const string& _last_updated_by)
		{ last_updated_by = _last_updated_by; }
	
	inline void setLastUpdated(const unsigned int _last_updated)
		{ last_updated = _last_updated; }
	
	inline void setAccess(const unsigned int _access)
		{ access = _access; }
	
	/* Convenience functions for flags */
	inline const bool isSuspended() const
		{ return (flags & F_SUSPENDED); }
	
	
	/* Methods to alter our SQL status */
	void setAllMembers(int);
	bool commit();

protected:
	string		user_name;
	unsigned int	last_seen;
	string		last_updated_by;
	unsigned int	last_updated;
	unsigned int	flags;
	unsigned int	access;
	
	PgDatabase*	SQLDb;
}; // class sqlUser

} // namespace ds

} // namespace gnuworld

#endif
