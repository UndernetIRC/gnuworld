/**
 * ccServer.h
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
 * $Id: ccServer.h,v 1.7 2003/06/28 01:21:19 dan_karrels Exp $
 */

#ifndef __CCSERVER_H
#define __CCSERVER_H "$Id: ccServer.h,v 1.7 2003/06/28 01:21:19 dan_karrels Exp $"

#include	<string>

#include	<ctime>

#include	"libpq++.h"
#include	"iServer.h"

namespace gnuworld
{ 

using std::string ;

namespace uworld
{

class ccServer
{
public:
	ccServer(PgDatabase*);
	virtual ~ccServer();
	//Mehods for getting info
	
	inline const string& 	getName() const
		{ return Name; }

	inline const string& 	getUplink() const
		{ return Uplink; }
	
	inline const string& 	getLastNumeric() const
		{ return Numeric; }
    
	inline const time_t&	getLastConnected() const
		{ return LastConnected; }
	    
	inline const time_t&	getLastSplitted() const
		{ return LastSplitted; }

	inline const string&	getSplitReason() const
		{ return SplitReason; }
	
	inline const string&   	getVersion() const
		{ return Version; }
		
	inline const time_t&	getAddedOn() const
		{ return AddedOn; }
	    
	inline const time_t&	getLastUpdated() const
		{ return LastUpdated; }
	
	inline const iServer* getNetServer() const
		{ return NetServer; }
		
	//Methods for setting info
	
	inline void 		setName( const string& _Name ) 
		{ Name = _Name; }

	inline void 		setUplink( const string& _Uplink ) 
		{ Uplink = _Uplink; }
	
	inline void 		setLastNumeric( const string& _LastNumeric ) 
		{ Numeric = _LastNumeric; }
    
	inline void		setLastConnected( const time_t& _LastConnected ) 
		{ LastConnected = _LastConnected; }
	    
	inline void		setLastSplitted( const time_t& _LastSplitted ) 
		{ LastSplitted = _LastSplitted; }

        inline void		setSplitReason( const string& _Reason)
		{ SplitReason = _Reason; }

	inline void		setVersion( const string& _Version )
		{ Version = _Version; }
		
	inline void		setAddedOn( const time_t& _AddedOn ) 
		{ AddedOn = _AddedOn; }

	inline void		setLastUpdated( const time_t& _LastUpdated ) 
		{ LastUpdated = _LastUpdated; }

	inline void		setNetServer(iServer* NewServer)
		{ NetServer = NewServer; }
		
	inline void 		setSqldb(PgDatabase* _SQLDb)
		{ SQLDb = _SQLDb; }
		
	//Methods for updating 

	bool Insert();
	
	bool Update();
	
	bool loadData( string );

	bool loadNumericData( string );

	bool loadDataFromDB(int place = 0 );
	
	bool Delete();

	static unsigned int numAllocated;
        
protected:
	string Name;
	string Uplink;
	string Numeric;
	time_t LastConnected;
	time_t LastSplitted;
	string SplitReason;
	string Version;
	time_t AddedOn;
	time_t LastUpdated;
	iServer* NetServer;
	PgDatabase* SQLDb;

}; // class ccServer
}
} // namespace gnuworld

#endif // __CCSERVER_H
