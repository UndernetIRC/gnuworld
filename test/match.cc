/**
 * match.cc
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
 * $Id: match.cc,v 1.4 2004/05/18 16:51:05 dan_karrels Exp $
 */

#include	<iostream>
#include	<string>

#include	"ELog.h"
#include	"match.h"

using namespace std ;
using namespace gnuworld ;

void getInput( string& s1, string& s2 )
{
cout	<< "Please input two strings to match: " ;
cout.flush() ;

cin	>> s1 >> s2 ;
}

int main()
{
string s1 ;
string s2 ;

getInput( s1, s2 ) ;

cout	<< "Matching "
	<< s1
	<< " and "
	<< s2
	<< ": " ;

int result = match( s1, s2 ) ;

if( 0 == result )
	{
	cout	<< "Match" ;
	}
else
	{
	cout	<< "No Match" ;
	}

cout	<< endl ;

return 0 ;
}
