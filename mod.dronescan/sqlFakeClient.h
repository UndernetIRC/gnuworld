#ifndef SQLFAKECLIENT_H
#define SQLFAKECLIENT_H "$Id: sqlFakeClient.h,v 1.2 2003/11/26 23:30:22 dan_karrels Exp $"

#include <string>

#include <ctime>

class PgDatabase;

namespace gnuworld {

namespace ds {

using std::string ;

class sqlFakeClient {
public:
	sqlFakeClient(PgDatabase*);
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
	
	PgDatabase *SQLDb;
}; // class sqlFakeClient

} // namespace ds

} // namespace gnuworld

#endif
