/* EWriteThread.cc */

#include	<string>

#include	<semaphore.h>
#include	<unistd.h> // usleep()

#include	"Socket.h"
#include	"EWriteThread.h"

using std::string ;

/// This method should only block for a very short period, if at all.
void EWriteThread::Write( const string& outString )
{
sem_wait( &bufferLock ) ;
*buffer += outString ;
sem_post( &bufferLock ) ;
}

/**
 * The run() method MUST respond (quickly if possible) to keepRunning.
 * When it turns to false, the thread must end itself.
 */
void EWriteThread::run()
{

// Need to make sure not to block for too long during the blocking
// write().
// ==> Write chunks of the buffer at once, leaving the semaphore
// unlocked as much as possible.

while( keepRunning )
	{
	usleep( 10000 ) ;

	}

}
