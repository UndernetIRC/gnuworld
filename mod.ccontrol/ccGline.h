/* ccGline.h */

#ifndef __CCGLINE_H
#define __CCGLINE_H "$Id: ccGline.h,v 1.11 2002/12/28 22:44:56 mrbean_ Exp $"

#include	<string>
#include	<list>

#include	<ctime>

#include	"libpq++.h"

namespace gnuworld
{ 

using std::string ;
using std::list ;
namespace uworld
{

class ccGline
{
public:
	typedef list<string*>	burstServersType;
	
	typedef burstServersType::iterator burstIterator;

	ccGline(PgDatabase*);
	
	virtual ~ccGline();
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
	    
	inline void 		setSqldb(PgDatabase* _SQLDb)
		{ SQLDb = _SQLDb; } 
		
	inline burstIterator 	getBurstBegin()
	{ return burstServers.begin(); }
	
	inline burstIterator	getBurstEnd()
	{ return burstServers.end(); }

	//Methods for updating 

	bool Insert();
	
	bool Update();
	
	bool loadData( int );

	bool loadData( const string & );
	
	bool Delete();
	
	void addBurst(string*);

	void clearBurst();

	static unsigned int numAllocated;
	
protected:
	string Id;
	string Host;
	string AddedBy;
	time_t AddedOn;
	time_t Expires;
	time_t LastUpdated;
	string Reason;
	burstServersType	burstServers;
	PgDatabase* SQLDb;
	
}; // class ccGline
}
} // namespace gnuworld

#endif // __CCGLINE_H
