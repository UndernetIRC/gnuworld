/**
 * gThread.cc
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<unistd.h>

#include	<string>
#include	<iostream>

#include	"ELog.h"
#include	"gThread.h"

using namespace std ;
using namespace gnuworld ;

ELog gnuworld::elog ;

class subThread1 : public gThread
{
public:
	virtual void Exec()
	{ while( keepRunning )
		{
		cout	<< "subThread1" << endl ;
		sleep( 1 ) ;
		}
	}
} ;

class subThread2 : public gThread
{
public:
	virtual void Exec()
	{ while( keepRunning )
		{
		cout	<< "subThread2" << endl ;
		sleep( 2 ) ;
		}
	}
} ;

class subThread3 : public gThread
{
public:
	virtual void Exec()
	{ while( keepRunning )
		{
		cout	<< "subThread3" << endl ;
		sleep( 3 ) ;
		}
	}
} ;

int main( int argc, const char** argv )
{

subThread1 t1 ;
subThread2 t2 ;
subThread3 t3 ;

//cout	<< "Executing t1.Exec()" << endl ;
//t1.Exec() ;
//t1.Stop() ;

cout	<< "Starting threads..." << endl ;
t1.Start() ;
t2.Start() ;
t3.Start() ;

// Wait for user to hit CTRL-C
while( true )
	{
	sleep( 1 ) ;
	}

return 0 ;
}
