/**
 * ccException.h
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
 * $Id: ccException.h,v 1.9 2003/06/28 01:21:19 dan_karrels Exp $
 */

#ifndef __CCEXCEPTION_H
#define __CCEXCEPTION_H "$Id: ccException.h,v 1.9 2003/06/28 01:21:19 dan_karrels Exp $"

#include	<string>

#include	<ctime>

#include	"libpq++.h"

#include	"match.h"

namespace gnuworld
{ 

using std::string ;

namespace uworld
{

class ccException
{
public:
	
	ccException(PgDatabase* _SQLDb);
	virtual ~ccException();
	
	inline bool operator==( const string& ExceptionHost ) const
		{ return (!strcasecmp( Host, ExceptionHost )
		    || !match(Host.c_str(),ExceptionHost.c_str())) ; }

	inline const string& 		getHost() const
			{ return Host; }
		
	inline const int		getConnections() const
		{ return Connections; }
	
	inline const string&		getAddedBy() const
		{ return AddedBy; }
		
	inline time_t			getAddedOn() const
		{ return AddedOn; }

	inline const string& 		getReason() const
			{ return Reason; }

	inline void 		setHost( const string& _Host ) 
		{ Host = _Host; }
		
	inline void		setConnections( const int _Connections ) 
		{  Connections = _Connections; }
	
	inline void		setAddedBy( const string& _AddedBy) 
		{ AddedBy = _AddedBy; }
		
	inline void		setAddedOn( const time_t _AddedOn )
		{ AddedOn = _AddedOn; }
	
	inline void 		setReason( const string& _Reason ) 
		{ Reason = _Reason; }

	inline void		setSqldb(PgDatabase* _SQLDb)
		{ SQLDb = _SQLDb; }
		
	int		loadData(const string& Host);
	
	int		updateData();
				
	bool		Insert();
	
	bool		Delete();
	
	static unsigned int numAllocated;

protected:
	
	string 		Host;
	int 		Connections;
	string 		AddedBy;
	time_t		AddedOn;
	string 		Reason;
	PgDatabase* SQLDb;

}; 
}
}
#endif // __CCEXCEPTION_H
