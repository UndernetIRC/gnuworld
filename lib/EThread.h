/* EThread.h */

#ifndef __ETHREAD_H
#define __ETHREAD_H

#include	<pthread.h>

/**
 * This is the abstract base class for all gnuworld threading.  It (nicely =P)
 * wraps a POSIX thread.  It is a once-use class.  That is, once you
 * have started and stopped the thread, it cannot be restarted.
 */
class EThread
{

protected:

	/**
	 * This is the data variable upon which the thread operates.
	 * POSIX threads receive a single void* argument.
	 */
	void*		data ;

	/**
	 * This variable is true while the thread is running, and false
	 * otherwise.
	 */
	bool		keepRunning ;

	/**
	 * This is the thread system identifier.
	 */
	pthread_t	threadID ;

	/**
	 * This variable is set to true when the thread starts, and set
	 * to false when it stops.  This is used to ensure that the thread
	 * is run only once.
	 */
	bool		hasRun ;

public:

	/**
	 * The EThread constructor stores the data variable, and initializes
	 * the state variables internally.
	 * The thread is initially NOT running.
	 */
	EThread( void* _data )
	 : data( _data ),
	   keepRunning( false ),
	   threadID( 0 ),
	   hasRun( false )
	{}

	/**
	 * Destroy the thread.
	 */
	virtual ~EThread()
	{ stop() ; }

	/**
	 * Call this method to invoke thread execution.
	 */
	virtual bool	start() ;

	/**
	 * Call this method to halt thread execution.  Once stop() has
	 * been called, the thread cannot restart.
	 */
	virtual void*	stop( bool doJoin = false ) ;

	/**
	 * This method returns true if the thread is currently running.
	 * It is not strictly threadsafe, but it is read-only, so it
	 * should be ok.
	 */
	inline bool	isRunning() const
		{ return keepRunning ; }

protected:

	/**
	 * This is the actual thread method.  This is pure virtual,
	 * so subclasses MUST overload this method for the thread to
	 * run correctly.  The run() method MUST respond to a change
	 * of keepRunning to false, that is, it cannot block indefinitely.
	 */
	virtual void	run() = 0 ;

	/**
	 * This is the C interface stub function.
	 * This method *always* returns NULL.
	 */
	static void* stub( void* ) ;

} ;

#endif // __ETHREAD_H
