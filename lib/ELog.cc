/* ELog.cc */

#include	<iostream>
#include	<fstream>
#include	<string>

#include	"config.h"
#include	"ELog.h"

const char ELog_h_rcsId[] = __ELOG_H ;
const char ELog_cc_rcsId[] = "$Id: ELog.cc,v 1.1 2000/11/06 16:08:33 dan_karrels Exp $" ;

using std::string ;

namespace gnuworld
{

ELog::ELog()
 : outStream( 0 )
{}

#ifdef EDEBUG
ELog::ELog( const string& fileName )
 : outStream( 0 )
{
openFile( fileName ) ;
}
#endif

ELog::~ELog()
{
#ifdef EDEBUG
  closeFile() ;
#endif
}

#ifdef EDEBUG
bool ELog::openFile( const string& fileName )
{
if( isOpen() )
	{
	closeFile() ;
	}
outFile.open( fileName.c_str() ) ;
return isOpen() ;
}

void ELog::closeFile()
{
if( isOpen() )
	{
	outFile << endl ;
	outFile.close() ;
	}
}
#endif // EDEBUG

ELog& ELog::operator<<( __E_omanip var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

ELog& ELog::operator<<( __E_manip var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

ELog& ELog::operator<<( const char& var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

ELog& ELog::operator<<( const string& var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

ELog& ELog::operator<<( const int& var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

ELog& ELog::operator<<( const unsigned int& var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

ELog& ELog::operator<<( const long int& var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

ELog& ELog::operator<<( const unsigned long int& var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

ELog& ELog::operator<<( const short int& var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

ELog& ELog::operator<<( const unsigned short int& var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

} // namespace gnuworld
