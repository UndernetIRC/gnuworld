/* ccServer.h */

#ifndef __CCSERVER_H
#define __CCSERVER_H "$Id: ccServer.h,v 1.1 2001/05/02 12:38:10 mrbean_ Exp $"

#include	<string>

#include	<ctime>

#include	"libpq++.h"

namespace gnuworld
{ 

using std::string ;

class ccServer
{
public:
	ccServer(PgDatabase*);
	virtual ~ccServer();
	//Mehods for getting info
	
	inline const string& 	get_Name() const
		{ return Name; }

	inline const string& 	get_Uplink() const
		{ return Uplink; }
	
	inline const string& 	get_LastNumeric() const
		{ return Numeric; }
    
	inline const time_t&	get_LastConnected() const
		{ return LastConnected; }
	    
	inline const time_t&	get_LastSplitted() const
		{ return LastSplitted; }

	
	//Methods for setting info
	
	inline void 		set_Name( const string& _Name ) 
		{ Name = _Name; }

	inline void 		set_Uplink( const string& _Uplink ) 
		{ Uplink = _Uplink; }
	
	inline void 		set_LastNumeric( const string& _LastNumeric ) 
		{ Numeric = _LastNumeric; }
    
	inline void		set_LastConnected( const time_t& _LastConnected ) 
		{ LastConnected = _LastConnected; }
	    
	inline void		set_LastSplitted( const time_t& _LastSplitted ) 
		{ LastSplitted = _LastSplitted; }

    
	//Methods for updating 

	bool Insert();
	
	bool Update();
	
	bool loadData( string );

	bool loadNumericData( string );

	bool Delete();
        
protected:
	string Name;
	string Uplink;
	string Numeric;
	time_t LastConnected;
	time_t LastSplitted;
	PgDatabase* SQLDb;

}; // class ccServer

} // namespace gnuworld

#endif // __CCSERVER_H
