/* ccServer.h */

#ifndef __CCSERVER_H
#define __CCSERVER_H "$Id: ccServer.h,v 1.5 2001/11/21 20:54:40 mrbean_ Exp $"

#include	<string>

#include	<ctime>

#include	"libpq++.h"

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

	//Methods for updating 

	bool Insert();
	
	bool Update();
	
	bool loadData( string );

	bool loadNumericData( string );

	bool Delete();

	static unsigned int numAllocated;
        
protected:
	string Name;
	string Uplink;
	string Numeric;
	time_t LastConnected;
	time_t LastSplitted;
	string SplitReason;
	PgDatabase* SQLDb;

}; // class ccServer
}
} // namespace gnuworld

#endif // __CCSERVER_H
