/* sqlLevel.h */

#ifndef __SQLLEVEL_H
#define __SQLLEVEL_H "$Id: sqlLevel.h,v 1.6 2001/01/05 06:44:05 gte Exp $"

#include	<string>
#include	<ctime>
#include	"libpq++.h"
 
using std::string ;

namespace gnuworld
{ 
 
class sqlLevel
{

public:
	sqlLevel(PgDatabase*) ;
	virtual ~sqlLevel() ;

	typedef unsigned int	flagType ; 
	static const flagType	F_AUTOOP =	0x01 ;
	static const flagType	F_PROTECT =	0x02 ;
	static const flagType	F_FORCED =	0x04 ;

	/*
	 *  Methods to get data atrributes.
	 */
 
	inline const unsigned int&	getChannelId() const
		{ return channel_id ; } 

	inline const unsigned int&	getUserId() const
		{ return user_id ; } 

	inline const unsigned short&	getAccess() const
		{ return access ; } 

	inline const flagType&	getFlags() const
		{ return flags ; }

	inline bool		getFlag( const flagType& whichFlag ) const
		{ return (flags & whichFlag) ; }

	inline const time_t&		getSuspendExpire() const
		{ return suspend_expires ; }

	inline const string&		getSuspendBy() const
		{ return suspend_by ; }

	inline const time_t&		getAdded() const
		{ return added ; }

	inline const string&		getAddedBy() const
		{ return added_by ; }

	inline const time_t&		getLastModif() const
		{ return last_modif ; }

	inline const string&		getLastModifBy() const
		{ return last_modif_by ; }

	inline const time_t&		getLastUpdated() const
		{ return last_updated ; }
	 
	bool loadData( unsigned int, unsigned int ); 
    void setAllMembers(int);

	/*
	 *  Methods to set data atrributes.
	 */
 
	inline void setChannelId( const unsigned int& _channel_id )
		{ channel_id = _channel_id; }
 	
	inline void setUserId( const unsigned int& _user_id )
		{ user_id = _user_id; }
 	
	inline void setAccess( const unsigned short& _access )
		{ access = _access; }
 
	inline void setFlag( const flagType& whichFlag )
		{ flags |= whichFlag; }
	
	inline void removeFlag( const flagType& whichFlag )
		{ flags &= ~whichFlag; }
	
	inline void setSuspendExpire( const time_t& _suspend_expires )
		{ suspend_expires = _suspend_expires; }
	
	inline void setSuspendBy( const string& _suspend_by )
		{ suspend_by = _suspend_by; } 
	
	inline void setAdded( const time_t& _added )
		{ added = _added; }

	inline void setAddedBy( const string& _added_by )
		{ added_by = _added_by; } 

	inline void setLastModif( const time_t& _last_modif )
		{ last_modif = _last_modif; }

	inline void setLastModifBy( const string& _last_modif_by )
		{ last_modif_by = _last_modif_by; } 
 
protected:
 
	unsigned int	channel_id ;
	unsigned int	user_id ;
	unsigned short	access ;
	flagType		flags ;
	time_t			suspend_expires ;
	string			suspend_by ; 
	time_t			added ;
	string			added_by ;
	time_t			last_modif ;
	string			last_modif_by ;
	time_t			last_updated ;
 
	PgDatabase*		SQLDb;
} ;

} 
#endif // __SQLLEVEL_H
