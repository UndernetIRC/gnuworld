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
 * $Id: gThread.h,v 1.1 2002/08/08 21:31:44 dan_karrels Exp $
 */

#ifndef __GTHREAD_H
#define __GTHREAD_H "$Id: gThread.h,v 1.1 2002/08/08 21:31:44 dan_karrels Exp $"

#include	<pthread.h>

#include	<map>
#include	<string>

#include	"misc.h"

namespace gnuworld
{

using std::map ;
using std::string ;

class gThread
{

protected:
	typedef map< string, pthread_mutex_t, noCaseCompare >	mutexMapType ;
	typedef mutexMapType::iterator		mutexIterator ;
	typedef mutexMapType::const_iterator	constMutexIterator ;

public:

	gThread( gThread* ) ;
	virtual ~gThread() ;

	virtual bool		Start() ;

	virtual bool		isRunning() const
		{ return (keepRunning && (threadID != 0)) ; }

	virtual void		shutDown()
		{ keepRunning = false ; }

	virtual void		Join() ;

	virtual bool		CreateMutex( const string& mutexName ) ;
	virtual bool		DestroyMutex( const string& mutexName ) ;
	virtual void		DestroyAllMutexes() ;

	/**
	 * Locks the mutex specified by mutexName.
	 * This method blocks indefinitely until the lock is obtained.
	 */
	virtual void		LockMutex( const string& mutexName ) ;

	/**
	 * Unlocks the mutex specified by mutexName.
	 * This method will return immediately.
	 */
	virtual void		UnLockMutex( const string& mutexName ) ;

	virtual void		Exec() = 0 ;

protected:

	bool			keepRunning ;
	bool			isShutdownComplete ;

	pthread_t		threadID ;
	mutexMapType		mutexMap ;
	pthread_mutex_t		waitMutex ;

} ;

} // namespace gnuworld

#endif // __GTHREAD_H
