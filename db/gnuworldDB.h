/* gnuworldDB.h */

#ifndef __GNUWORLDDB_H
#define __GNUWORLDDB_H "$Id: gnuworldDB.h,v 1.1 2001/05/11 21:43:39 dan_karrels Exp $"

#include	<string>

#include	<sys/types.h>

namespace gnuworld
{

using std::string ;

class gnuworldDB
{

public:

	static const bool	SUCCESS = true ;
	static const bool	FAILURE = false ;

	virtual bool		Exec( const string& ) = 0 ;
	virtual bool		isConnected() const = 0 ;

	virtual unsigned int	countTuples() const = 0 ;
	virtual pid_t		getPID() const = 0 ;

	virtual const string&	ErrorMessage() const = 0 ;
	virtual const string&	GetValue( const unsigned int&,
					const unsigned int& ) const = 0 ;

protected:


} ;

} // namespace gnuworld

#endif // __GNUWORLDDB_H
