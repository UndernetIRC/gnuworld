/**
 * econfig.cc
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<string>
#include	<iostream>

#include	"EConfig.h"
#include	"ELog.h"

using namespace std ;
using namespace gnuworld ;

ELog gnuworld::elog ;

int main( int argc, const char** argv )
{
if( argc != 2 )
	{
	cout	<< "Usage: "
		<< argv[ 0 ]
		<< " <conf file>"
		<< endl ;
	return -1 ;
	}

EConfig conf( argv[ 1 ] ) ;

for( EConfig::const_iterator itr = conf.Require( "you" ) ;
	(itr != conf.end()) && (itr->first == "you") ;
	++itr )
	{
	cout	<< "Key: "
		<< itr->first
		<< ", Value: "
		<< itr->second
		<< endl ;
	}

for( EConfig::const_iterator itr = conf.Require( "had" ) ;
	(itr != conf.end()) && (itr->first == "had") ;
	++itr )
	{
	cout	<< "Key: "
		<< itr->first
		<< ", Value: "
		<< itr->second
		<< endl ;
	}

return 0 ;
}
