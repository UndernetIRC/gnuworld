
#include	<iostream>
#include	<string>

#include	"ELog.h"
#include	"match.h"

using namespace std ;
using namespace gnuworld ;

ELog elog ;

void getInput( string& s1, string& s2 )
{
cout	<< "Please input two strings to match: " ;
cout.flush() ;

cin	>> s1 >> s2 ;
}

int main( int argc, const char** argv )
{

string s1 ;
string s2 ;

if( argc != 3 )
	{
	getInput( s1, s2 ) ;
	}

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
