/* sqlBan.h */

#ifndef __SQLBAN_H
#define __SQLBAN_H "$Id: sqlBan.h,v 1.5 2001/02/18 19:46:01 dan_karrels Exp $"

#include	<string>
#include	<ctime>
#include	"libpq++.h"
 
using std::string ;

namespace gnuworld
{ 
 
class sqlBan
{

public:
	sqlBan(PgDatabase*) ;
	virtual ~sqlBan() ;
 
	/*
	 *  Methods to get data atrributes.
	 */
 
	inline const unsigned int&	getID() const
		{ return id ; } 

	inline const unsigned int&	getChannelID() const
		{ return channel_id ; } 

	inline const string&		getBanMask() const
		{ return banmask ; }

	inline const string&		getSetBy() const
		{ return set_by ; }

	inline const time_t&		getSetTS() const
		{ return set_ts ; }

	inline const unsigned short&	getLevel() const
		{ return level ; } 

	inline const time_t&	getExpires() const
		{ return expires ; } 
 
	inline const string&		getReason() const
		{ return reason ; }

	inline const time_t&		getLastUpdated() const
		{ return last_updated ; }
	 
	/*
	 *  Methods to set data atrributes.
	 */
 
	inline void setChannelID( const unsigned int& _channel_id )
		{ channel_id = _channel_id; } 
 	
	inline void setBanMask( const string& _banmask )
		{ banmask = _banmask; } 

	inline void setSetBy( const string& _set_by )
		{ set_by = _set_by; } 

	inline void setSetTS( const time_t& _set_ts )
		{ set_ts = _set_ts; } 
		
	inline void setLevel( const unsigned short& _level )
		{ level = _level; }

	inline void setExpires( const unsigned int& _expires )
		{ expires = _expires; } 

	inline void setReason( const string& _reason )
		{ reason = _reason; } 
 
	bool commit();
	bool insertRecord();
	bool deleteRecord();

	void setAllMembers(int);
		
protected:
 
	unsigned int	id ; 
	unsigned int	channel_id ;
	string		banmask ;
	string		set_by ;
	time_t		set_ts ;
	unsigned short	level ;
	time_t		expires ;
	string		reason ; 
	time_t		last_updated ;
 
	PgDatabase*	SQLDb;
} ;

} 
#endif // __SQLBAN_H
