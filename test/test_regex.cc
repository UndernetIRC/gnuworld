
#include	<iostream>
#include	<string>

#include	<cstring>

#include	<pcre.h>

using namespace std ;

int main()
{
string line ;

cout	<< "Enter the regex to compile: " ;
cout.flush() ;
std::getline( cin, line ) ;

int errorOffset = 0 ;
const char* errorBuf = 0 ;

pcre* pPtr = pcre_compile( line.c_str(),
	PCRE_EXTENDED,
	&errorBuf,
	&errorOffset,
	0 ) ;
if( 0 == pPtr )
	{
	cout	<< "Unable to compile: "
		<< errorBuf
		<< endl ;
	return 0 ;
	}

cout	<< "Input string to match: " ;
cout.flush() ;

while( std::getline( cin, line ) )
	{
	int ovector[ 30 ] ;
	memset( &ovector, 0, sizeof( ovector ) ) ;

	int rc = pcre_exec( pPtr,
		NULL,
		line.c_str(),
		line.size(),
		0,
		0,
		ovector,
		30 ) ;
	if( rc < 0 )
		{
		cout	<< "NO Match"
			<< endl ;
		}
	else
		{
		cout	<< "Match, ovector[0]: "
			<< ovector[0]
			<< ", ovector[1]: "
			<< ovector[1]
			<< endl ;
		}

	cout	<< "Input string to match: " ;
	cout.flush() ;

	} // while( getline() )
return 0 ;
}
