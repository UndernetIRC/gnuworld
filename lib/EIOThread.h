/* EIOThread.h */

#ifndef __EIOTHREAD_H
#define __EIOTHREAD_H

#include	<semaphore.h>

#include	"Socket.h"
#include	"Buffer.h"
#include	"EThread.h"

/**
 * This is the abstract base class for all of gnuworld's IO communications.
 */
class EIOThread : public EThread
{

protected:

	/**
	 * A pointer to the hardware communications channel.
	 */
	Socket*		sock ;

	/**
	 * A pointer to the input/output buffer.
	 */
	Buffer*		buffer ;

	/**
	 * A locking semaphore for access to the Buffer.
	 */
	sem_t		bufferLock ;

public:

	/**
	 * This constructor calls the base class (EThread) constructor, and
	 * allocates the I/O buffer.
	 */
	EIOThread( void* _data ) ;

	/**
	 * The destructor deallocates the Buffer.
	 */
	virtual ~EIOThread() ;

} ;

#endif // __EIOTHREAD_H
