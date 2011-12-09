/**
 * csGline.h
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
 */

#ifndef __CSGLINE_H
#define __CSGLINE_H "$Id: csGline.h, 06/10/2011 Spike $"

#include	<string>
#include	<list>

#include	<ctime>

#include	"dbHandle.h"

namespace gnuworld
{ 

using std::string ;
using std::list ;

class csGline
{
public:

	csGline(dbHandle*);
	
	virtual ~csGline();
	//Mehods for getting info
	
	inline const string& 	getId() const
		{ return Id; }

	inline const string& 	getHost() const
		{ return Host; }
	
	inline const string& 	getAddedBy() const
		{ return AddedBy; }
    
	inline const time_t&	getAddedOn() const
		{ return AddedOn; }
	    
	inline const time_t&	getExpires() const
		{ return Expires; }

	inline const time_t&	getLastUpdated() const
		{ return LastUpdated; }

	inline const string&	getReason() const
		{ return Reason; }

	
	//Methods for setting info
	
	inline void 		setId( const string& _Id ) 
		{ Id = _Id; }

	inline void 		setHost( const string& _Host ) 
		{ Host = _Host; }
	
	inline void 		setAddedBy( const string& _AddedBy ) 
		{ AddedBy = _AddedBy; }
    
	inline void		setAddedOn( const time_t& _AddedOn ) 
		{ AddedOn = _AddedOn; }
	    
	inline void		setExpires( const time_t& _Expires ) 
		{ Expires = _Expires; }

	inline void		setLastUpdated( const time_t& _LastUpdated ) 
		{ LastUpdated = _LastUpdated; }

	inline void 		setReason( const string& _Reason ) 
		{ Reason = _Reason; }
	    
	inline void 		setSqldb(dbHandle* _SQLDb)
		{ SQLDb = _SQLDb; } 
		
	//Methods for updating 

	bool Insert();
	
	bool Update();
	
	bool loadData( int );

	bool loadData( const string & );
	
	bool Delete();
	
	static unsigned int numAllocated;
	
protected:
	string Id;
	string Host;
	string AddedBy;
	time_t AddedOn;
	time_t Expires;
	time_t LastUpdated;
	string Reason;
	dbHandle* SQLDb;
	
}; // class csGline
} // namespace gnuworld

#endif
