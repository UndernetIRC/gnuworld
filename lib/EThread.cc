/* EThread.cc */

#include	<iostream>
#include	<cstring>
#include	<cerrno>
#include	<pthread.h>

#include	"EThread.h"
#include	"ELog.h"

using std::endl ;
using gnuworld::elog ;

void* EThread::stop( bool doJoin )
{

if( !keepRunning )
	{
	// Thread is not running
	return 0 ;
	}

void* retMe = 0 ;
keepRunning = false ;
hasRun = true ;

if( doJoin )
	{
	pthread_join( threadID, &retMe ) ;
	}

threadID = 0 ;
return retMe ;
}

bool EThread::start()
{
if( keepRunning )
	{
	// Thread is already running, NO-OP.
	return true ;
	}

if( hasRun )
	{
	// Thread has already run once previously.
	// Only allow the thread to run once.
	return false ;
	}

keepRunning = true ;

if( pthread_create( &threadID, 0,
	static_cast< void* (*)(void*) >( stub ),
	this ) != 0 )
	{
	elog	<< "EThread::start()> Error in pthread_create: "
		<< strerror( errno ) << endl ;
	keepRunning = false ;
	return false ;
	}

return true ;
}

void* EThread::stub( void* voidThread )
{
EThread* theThread = static_cast< EThread* >( voidThread ) ;
if( NULL == theThread )
	{
	elog	<< "EThread::stub()> Unable to cast to EThread*\n" ;
	// TODO: Should this exit()?
	return 0 ;
	}
theThread->run() ;
return 0 ;
}
