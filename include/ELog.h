/* ELog.h */

#ifndef __ELOG_H
#define __ELOG_H "$Id: ELog.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#include	<iostream>
#include	<fstream>
#include	<string>

#include	"config.h"

using std::ofstream ;
using std::ostream ;
using std::string ;
using std::ios ;

namespace gnuworld
{

class ELog
{

protected:

	typedef ostream& (*__E_omanip)( ostream& ) ;
	typedef ostream& (*__E_manip)( ios& ) ;

	ostream		*outStream ;
#ifdef EDEBUG
	ofstream	outFile ;
#endif

public:
	ELog() ;
#ifdef EDEBUG
	ELog( const string& ) ;
#endif
	virtual ~ELog() ;

#ifdef EDEBUG
	inline bool openFile( const string& fileName ) ;
	inline void closeFile() ;
	inline bool isOpen() const
		{ return outFile.is_open() ; }
#endif

	inline void setStream( ostream* newStream )
		{ outStream = newStream ; }

	inline ELog& operator<<( __E_omanip func ) ;
	inline ELog& operator<<( __E_manip func ) ;
	inline ELog& operator<<( const char& ) ;
	inline ELog& operator<<( const string& ) ;
	inline ELog& operator<<( const int& ) ;
	inline ELog& operator<<( const unsigned int& ) ;
	inline ELog& operator<<( const long int& ) ;
	inline ELog& operator<<( const unsigned long int& ) ;
	inline ELog& operator<<( const short int& ) ;
	inline ELog& operator<<( const unsigned short int& ) ;

} ;

extern ELog elog ;

} // namespace gnuworld

#endif // __ELOG_H
