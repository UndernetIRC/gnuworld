/* sqlLevel.h */

#ifndef __SQLLEVEL_H
#define __SQLLEVEL_H "$Id: sqlLevel.h,v 1.1 2000/12/22 00:29:32 gte Exp $"

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
	inline const time_t&		getLastUpdate() const
		{ return last_update ; }
 
	bool loadData( unsigned int, unsigned int ); 

protected:

    void setAllMembers();

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
	time_t			last_update ;

	PgDatabase*		SQLDb;
} ;

} 
#endif // __SQLLEVEL_H
