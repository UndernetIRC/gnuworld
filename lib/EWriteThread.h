/* EWriteThread.h */

#ifndef __EWRITETHREAD_H
#define __EWRITETHREAD_H

#include	<string>

#include	"EIOThread.h"

using std::string ;

/**
 * This subclass of EIOThread is responsible for writing to
 * a Socket.
 */
class EWriteThread : public EIOThread
{

public:

	/**
	 * Basic constructor, just call base class constructor.
	 */
	EWriteThread( void* _data )
	 : EIOThread( _data )
	{}

	/**
	 * Basic destructor, no memory allocated in this class.
	 */
	virtual ~EWriteThread()
	{}

	/**
	 * Add a std::string to the thread output buffer.
	 */
	virtual void	Write( const string& ) ;

protected:

	/**
	 * Overloaded version of run() method, this is the core of
	 * this thread's functionality.
	 */
	virtual void	run() ;

} ;

#endif // __EWRITETHREAD_H
