/**
 * gThread.h
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 * 
 * $Id: gThread.h,v 1.5 2003/12/29 23:59:36 dan_karrels Exp $
 */

#ifndef __GTHREAD_H
#define __GTHREAD_H "$Id: gThread.h,v 1.5 2003/12/29 23:59:36 dan_karrels Exp $"

#include	<pthread.h>

#include	<map>
#include	<string>

#include	"misc.h"

namespace gnuworld
{

/**
 * The base class for creating a thread.
 * Simply subclass this class and overload the Exec()
 * method, and call Start() to have a running thread.
 * Once started, each thread is responsible for monitoring
 * this object's 'keepRunning' variable.  It should
 * poll this variable (note: do not block indefinitely)
 * and return from its Exec() method when keepRunning
 * is set to false; this indicates that a Stop() request
 * has been issued.
 */
class gThread
{

protected:
	/**
	 * The type used to store mutexes.
	 */
	typedef std::map< std::string, pthread_mutex_t, noCaseCompare >	
			mutexMapType ;

	/**
	 * The iterator type to the mutex map.
	 */
	typedef mutexMapType::iterator		mutexIterator ;

	/**
	 * The const_iterator type to the mutex map.
	 */
	typedef mutexMapType::const_iterator	constMutexIterator ;

public:

	/**
	 * Default constructor.
	 */
	gThread() ;

	/**
	 * The destructor will stop the thread, issue a join(), and
	 * destroy any mutexes.
	 */
	virtual ~gThread() ;

	/**
	 * Invoke this method to begin the Exec() method in a new
	 * thread.
	 */
	virtual bool		Start() ;

	/**
	 * Return true if this thread is running, false otherwise.
	 */
	virtual bool		isRunning() const
		{ return (keepRunning && (threadID != 0)) ; }

	/**
	 * Request that this thread stop processing.  This could take
	 * an arbitrary length of time to fully stop, depending on how
	 * well the thread subclass follows directions (see above).
	 */
	virtual void		Stop()
		{ keepRunning = false ; }

	/**
	 * Issue a pthread_join() on this (already stopped) thread to
	 * cleanup.
	 */
	virtual void		Join() ;

	/**
	 * Create a mutex with the given case insensitive name.
	 * True is returned if successfull, false otherwise.
	 */
	virtual bool		CreateMutex( const std::string& mutexName ) ;

	/**
	 * Destroy a mutex with the given case insensitive name.
	 * True is returned if successfull, false otherwise.
	 */
	virtual bool		DestroyMutex( const std::string& mutexName ) ;

	/**
	 * Destroy all active mutexes.
	 */
	virtual void		DestroyAllMutexes() ;

	/**
	 * Locks the mutex specified by mutexName.
	 * This method blocks indefinitely until the lock is obtained.
	 */
	virtual void		LockMutex( const std::string& mutexName ) 
;

	/**
	 * Unlocks the mutex specified by mutexName.
	 * This method will return immediately.
	 */
	virtual void		UnLockMutex( const std::string& mutexName ) ;

	/**
	 * This method is invoked in a new thread when the Start()
	 * method is invoked.  Subclasses must overload this method
	 * to perform the meaningful duty of the thread proper.
	 * This method must halt when keepRunning is set to false.
	 */
	virtual void		Exec() {}

protected:

	/**
	 * The running status of the thread.  Subclasses must honor
	 * the value of this variable.
	 */
	bool			keepRunning ;

	/**
	 * True if shutdown has completed (keepRunning request has
	 * been honored), and it is clear to issue a join(), false
	 * otherwise.
	 */
	bool			isShutdownComplete ;

	/**
	 * The thread ID for this thread, 0 if not running.
	 */
	pthread_t		threadID ;

	/**
	 * The container used to store the active mutexes.
	 */
	mutexMapType		mutexMap ;

	/**
	 * A general purpose mutex on which to wait.
	 */
	pthread_mutex_t		waitMutex ;

} ; // class gThread

} // namespace gnuworld

#endif // __GTHREAD_H
