/* sqlChannel.h */

#ifndef __SQLCHANNEL_H
#define __SQLCHANNEL_H "$Id: sqlChannel.h,v 1.3 2000/12/11 21:36:09 gte Exp $"

#include	<string>

#include	<ctime>
#include	"libpq++.h"

using std::string ;

namespace gnuworld
{ 
 
class sqlChannel
{

public:
	sqlChannel(PgDatabase* SQLDb) ;
	virtual ~sqlChannel() ;

	typedef unsigned int	flagType ;

	static const flagType	F_PURGE =	0x01 ;
	static const flagType	F_SPECIAL =	0x02 ;
	static const flagType	F_NOREG =	0x04 ;
	static const flagType	F_NEVREG =	0x08 ;
	static const flagType	F_SUSPEND =	0x10 ;
	static const flagType	F_TEMP =	0x20 ;
	static const flagType	F_CAUTION =	0x40 ;
	static const flagType	F_VACATION =	0x80 ;
	static const flagType	F_ALWAYSOP =	0x1000 ;
	static const flagType	F_STRICTOP =	0x2000 ;
	static const flagType	F_NOOP =	0x4000 ;
	static const flagType	F_AUTOTOPIC =	0x8000 ;
	static const flagType	F_OPONLY =	0x100000 ;
	static const flagType	F_AUTOJOIN =	0x200000 ;

	inline const unsigned int&	getID() const
		{ return id ; }
	inline const string&		getName() const
		{ return name ; }
	inline const flagType&		getFlags() const
		{ return flags ; }
	inline bool		getFlag( const flagType& whichFlag ) const
		{ return (flags & whichFlag) ; }
	inline const unsigned short int& getMassDeopPro() const
		{ return mass_deop_pro ; }
	inline const unsigned short int& getFloodPro() const
		{ return flood_pro ; }
	inline const string&		getURL() const
		{ return url ; }
	inline const string&		getDescription() const
		{ return description ; }
	inline const string&		getKeywords() const
		{ return keywords ; }
	inline const time_t&		getRegisteredTS() const
		{ return registered_ts ; }
	inline const string&		getChannelMode() const
		{ return channel_mode ; }
	inline const string&		getChannelKey() const
		{ return channel_key ; }
	inline const unsigned int&	getChannelLimit() const
		{ return channel_limit ; }
	inline const time_t&		getLastUpdate() const
		{ return last_update ; }

	bool loadData( const string& );
	bool loadData( int );
	bool exists( const string& );

protected:

    void setAllMembers();

	unsigned int	id ;
	string		name ;
	flagType	flags ;
	unsigned short	mass_deop_pro ;
	unsigned short	flood_pro ;
	string		url ;
	string		description ;
	string		keywords  ;
	time_t		registered_ts ;
	time_t		channel_ts ;
	string		channel_mode ;
	string		channel_key ;
	unsigned int	channel_limit ;
	time_t		last_update ; 
	PgDatabase*	SQLDb;
} ;

} 
#endif // __SQLCHANNEL_H
