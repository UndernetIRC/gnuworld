/* stats.h */

#ifndef __STATS_H
#define __STATS_H

#include	<string>
#include	<map>

#include	"client.h"
#include	"iClient.h"
#include	"misc.h" // noCaseCompare
#include	"server.h"

using std::string ;
using std::map ;

namespace gnuworld
{

class stats : public xClient
{

protected:
	typedef map< string, unsigned long, noCaseCompare > mapType ;

public:
	stats( const string& ) ;
	virtual ~stats() ;

	typedef mapType::const_iterator const_iterator ;

	inline unsigned long& operator[]( const string& name )
		{ return table[ name ] ; }

	inline const_iterator begin() const
		{ return table.begin() ; }
	inline const_iterator end() const
		{ return table.end() ; }

	virtual void ImplementServer( xServer* ) ;
	virtual int OnPrivateMessage( iClient*, const string& ) ;
	virtual int OnEvent( const eventType&,
		void* = 0, void* = 0, void* = 0, void* = 0 ) ;

protected:
	mapType		table ;


} ;

} // namespace gnuworld

#endif // __STATS_h
