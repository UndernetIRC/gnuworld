#ifndef SQLFAKECLIENT_H
#define SQLFAKECLIENT_H "$Id: sqlFakeClient.h,v 1.1 2003/10/12 22:21:25 jeekay Exp $"

#include <string>

class PgDatabase;

namespace gnuworld {

namespace ds {

class sqlFakeClient {
public:
	sqlFakeClient(PgDatabase*);
	virtual ~sqlFakeClient();
	
	typedef unsigned int flagType;
	static const flagType F_ACTIVE;
	
	/* Accessors */
	inline unsigned int getId()
		{ return id; }
	
	inline string getNickName()
		{ return nickname; }
	
	inline string getUserName()
		{ return username; }
	
	inline string getHostName()
		{ return hostname; }
	
	inline string getRealName()
		{ return realname; }
	
	inline string getCreatedBy()
		{ return createdBy_s; }
	
	inline time_t getCreatedOn()
		{ return createdOn; }
	
	inline time_t getLastUpdated()
		{ return lastUpdated; }
	
	inline bool getFlag(flagType _flag)
		{ return (flags & _flag); }
	
	/* Mutators */
	inline void setNickName(string _nickname)
		{ nickname = _nickname; }
	
	inline void setUserName(string _username)
		{ username = _username; }
	
	inline void setHostName(string _hostname)
		{ hostname = _hostname; }
	
	inline void setRealName(string _realname)
		{ realname = _realname; }
	
	inline void setCreatedBy(int _createdby)
		{ createdBy_i = _createdby; }
	
	inline void setFlag(flagType _flag)
		{ flags |= _flag; }
	
	inline void removeFlag(flagType _flag)
		{ flags &= !_flag; }
	
	/* Convenience functions for flags */
	string getFlagsString();
	
	inline bool isActive()
		{ return (flags & F_ACTIVE); }
	
	/* Core */
	void setAllMembers(int);
	bool commit();
	bool insert();
	bool remove();
	
	/* Misc helper functions */
	string getNickUserHost();
	
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
