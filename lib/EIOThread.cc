/* EIOThread.cc */

#include	<new>
#include	<iostream>
#include	<stdexcept>

#include	<semaphore.h>
#include	<cstring>
#include	<cerrno>

#include	"Socket.h"
#include	"EIOThread.h"
#include	"ELog.h"

using std::endl ;
using gnuworld::elog ;

EIOThread::EIOThread( void* _data )
 : EThread( _data )
{
sock = static_cast< Socket* >( data ) ;
if( NULL == sock )
	{
	elog	<< "EIOThread> Unable to retrieve Socket\n" ;
	throw exception() ;
	}

try
	{
	buffer = new Buffer() ;
	}
catch( std::bad_alloc )
	{
	elog	<< "EIOThread> Memory allocation failure\n" ;
	exit( 0 ) ;
	}

// Initialize the Buffer semaphore to unlocked.
if( sem_init( &bufferLock, 0, 1 ) < 0 )
	{
	elog	<< "EIOThread> Failed to initialize the semaphore: "
		<< strerror( errno ) << endl ;
	exit( 0 ) ;
	}

}

EIOThread::~EIOThread()
{
delete buffer ;

// TODO: Check that no threads are waiting on the semaphore.
if( sem_destroy( &bufferLock ) < 0 )
	{
	elog	<< "~EIOThread> Unable to deallocate semaphore: "
		<< strerror( errno ) << endl ;
	// Non-fatal error
	}
}
