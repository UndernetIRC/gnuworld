/**
 * sqlBan.h
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
 * $Id: sqlBan.h,v 1.8 2009/06/25 19:05:23 mrbean_ Exp $
 */

#ifndef __SQLBAN_H
#define __SQLBAN_H "$Id: sqlBan.h,v 1.8 2009/06/25 19:05:23 mrbean_ Exp $"

#include	<string>
#include	<ctime>
#include	"dbHandle.h"
#include	"banMatcher.h"
 
using std::string ;

namespace gnuworld
{ 
 
class sqlBan
{

public:
	sqlBan(dbHandle*) ;
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

	inline banMatcher* getMatcher() const
		{ return matcher; }
		
	/*
	 *  Methods to set data atrributes.
	 */
 
	inline void setChannelID( const unsigned int& _channel_id )
		{ channel_id = _channel_id; } 
 	
	inline void setBanMask( const string& _banmask )
		{ banmask = _banmask; 
		  initMatcher();} 

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
	
	void initMatcher();
		
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
	
	dbHandle*	SQLDb;
	banMatcher*	matcher;
} ;

} 
#endif // __SQLBAN_H
