/**
 * ELog.cc
 * Author: Daniel Karrels (dan@karrels.com)
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: ELog.cc,v 1.2 2003/06/17 15:13:53 dan_karrels Exp $
 */

#include	<iostream>
#include	<fstream>
#include	<sstream>
#include	<string>

#include	"config.h"
#include	"ELog.h"

const char rcsId[] = "$Id: ELog.cc,v 1.2 2003/06/17 15:13:53 dan_karrels Exp $" ;

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

ELog& ELog::operator<<( const double& var )
{
#ifdef EDEBUG
  outFile << var ;
#endif
if( outStream ) *outStream << var ;
return *this ;
}

} // namespace gnuworld
