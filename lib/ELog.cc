/**
 * ELog.cc
 * Author: Daniel Karrels (dan@karrels.com)
 * $Id: ELog.cc,v 1.5 2002/05/23 17:43:10 dan_karrels Exp $
 */

#include	<iostream>
#include	<fstream>
#include	<sstream>
#include	<string>

#include	"config.h"
#include	"ELog.h"

const char ELog_h_rcsId[] = __ELOG_H ;
const char ELog_cc_rcsId[] = "$Id: ELog.cc,v 1.5 2002/05/23 17:43:10 dan_karrels Exp $" ;
const char config_h_rcsId[] = __CONFIG_H ;

namespace gnuworld
{

using std::stringstream ;
using std::string ;
using std::endl ;

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

ELog& ELog::operator<<( stringstream& s )
{
#ifdef EDEBUG
  outFile << s.str() ;
#endif
if( outStream ) *outStream << s.str() ;
return *this ;
}

} // namespace gnuworld
