/* EReadThread.cc */

#include	<string>
#include	<stdexcept>

#include	<semaphore.h>
#include	<unistd.h> // usleep()

#include	"Socket.h"
#include	"EReadThread.h"

using std::string ;

bool EReadThread::ReadLine( string& theString )
{
// This is not strictly legal, it assumes at least a small amount
// of thread safety in Buffer::empty()...It should be ok.
if( buffer->empty() )
	{
	return false ;
	}

bool retMe = true ;

sem_wait( &bufferLock ) ;
retMe = buffer->ReadLine( theString ) ;
sem_post( &bufferLock ) ;

return retMe ;
}

/**
 * The run() method MUST respond (quickly if possible) to keepRunning.
 * When it turns to false, the thread must end itself.
 */
void EReadThread::run()
{

// Semaphore blocking should be a problem with this thread because
// the semaphore is only locked momentarily while new data is
// written to the buffer.

while( keepRunning )
	{
	usleep( 10000 ) ;

	}

}
