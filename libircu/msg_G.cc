/**
 * msg_G.cc
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
 * $Id: msg_G.cc,v 1.8 2009/07/25 18:12:34 hidden1 Exp $
 */

#include	<sys/time.h>

#include	<string>
#include	<sstream>
#include	<iostream>

#include	<ctime>
#include	<cerrno>
#include	<cstring>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"xparameters.h"
#include	"ELog.h"
#include	"ServerCommandHandler.h"
#include	"StringTokenizer.h"

RCSTAG( "$Id: msg_G.cc,v 1.8 2009/07/25 18:12:34 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::stringstream ;

CREATE_HANDLER(msg_G)

// Q G :ripper.ufl.edu
// Q: Remote server numeric
// G: PING
// :ripper.ufl.edu: Ping argument
// Reply with:
// <Our Numeric> Z <Their Numeric> :<arguments>
// Strings will be passed to this method in format:
// Q ripper.ufl.edu
//
// New style ping:
// MJ G !1026249984.71811 test.gnuworld.org 1026249984.71811
// Here, params contains all but the 'G', and does include the '!'
//
// Reply with:
// <my_numeric> Z <remote_numeric> !<remotets> <difference> <localts>
// <my_numeric> Z <my_numeric> <remote_numeric> !<remotets> <difference> <localts>
// The format of <remotets> = <seconds>.<useconds>
// 
bool msg_G::Execute( const xParameters& params )
{
if( params.size() < 2 )
	{
	elog	<< "msg_G> Invalid number of parameters"
		<< endl ;
	return false ;
	}

string s( theServer->getCharYY() ) ;
s += " Z " ;

if( 2 == params.size() )
	{
	// Old style ping
	s += params[ 0 ] ;

	if( params.size() >= static_cast< xParameters::size_type >( 1 ) )
		{
		s += " :" ;
		s += params[ 1 ] ;
		}
	}
else
	{
	// New style ping
	// This little algorithm is purposely verbose
	// MJ !1026249984.71811 test.gnuworld.org 1026249984.71811
	// <my_numeric> Z <remote_numeric> !<remotets> <difference> <localts>
//	elog	<< "msg_G> New style ping"
//		<< endl ;

	s += theServer->getCharYY();
	s += " " ;
	// Target server
	//s += params[ 0 ] ;
	//s += " " ;

	// Remote TS, including the '!'
	s += params[ 1 ] ;
	s += " " ;
	string tStr = params[ 1 ];
	s += tStr.substr(1,string::npos) ;
	s += " " ;

	double remoteTSDouble = ::atof( params[ 1 ] + 1 ) ;

//	elog	<< "remoteTSDouble: "
//		<< ((int) remoteTSDouble)
//		<< "."
//		<< (remoteTSDouble - (int) remoteTSDouble)
//		<< endl ;

	timeval now = { 0, 0 } ;
	if( ::gettimeofday( &now, 0 ) < 0 )
		{
		elog	<< "msg_G> gettimeofday() failed: "
			<< strerror( errno )
			<< endl ;
		return false ;
		}


	StringTokenizer st(params[1] + 1, '.');
	if (st.size() != 2) {
		elog << "msg_G> Error in Remote TS" << endl;
		return false;
	}
	int tsDiff = (now.tv_sec - atoi(st[0])) * 1000 + (now.tv_usec - atoi(st[1])) / 1000;
	//elog << "msg_G> tsDiff = " << tsDiff << endl;

	stringstream	theStream ;
	theStream	<< now.tv_sec
			<< "."
			<< now.tv_usec ;

	string localTSString = theStream.str();
	// Obtain localTS as double val
	double localTSDouble = 0.0 ;
	theStream >> localTSDouble ;

//	elog	<< "msg_G> localTSString: "
//		<< localTSString
//		<< endl ;

	//double tsDiffDouble = localTSDouble - remoteTSDouble ;
	//double tsDiffD = 1000 * (localTSDouble - remoteTSDouble);
	//int tsDiff = static_cast<int>(tsDiffD);

//	elog	<< "msg_G> tsDiffDouble: "
//		<< tsDiffDouble
//		<< endl ;

	stringstream stream2 ;
	stream2		<< tsDiff ;

//	elog	<< "msg_G> stream2: "
//		<< stream2.str()
//		<< endl ;

	// Difference TS
	s += stream2.str() + " " ;

	// Local TS
	s += localTSString ;
	}

//elog << "msg_G> Message: " << s << endl;
return theServer->Write( s ) ;
}

} // namespace gnuworld
