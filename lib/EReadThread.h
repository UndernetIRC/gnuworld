/* EReadThread.h */

#ifndef __EREADTHREAD_H
#define __EREADTHREAD_H

#include	<string>

#include	"EIOThread.h"

using std::string ;

/**
 * This class is responsible for threaded reading of a communications
 * Socket.
 */
class EReadThread : public EIOThread
{

public:
	/**
	 * Basic constructor, just call base class constructor.
	 */
	EReadThread( void* _data )
	 : EIOThread( _data )
	{}

	/**
	 * Basic destructor, no memory allocated in this class.
	 */
	virtual ~EReadThread()
	{}

	/**
	 * Read a \n terminated from the buffer into the given std::string.
	 * Return true if a line was read, false otherwise.
	 */
	virtual bool	ReadLine( string& ) ;

protected:

	/**
	 * Overloaded version of EThread::run(), this is the core of
	 * this thread's functionality.
	 */
	virtual void	run() ;

} ;

#endif // __EREADTHREAD_H
