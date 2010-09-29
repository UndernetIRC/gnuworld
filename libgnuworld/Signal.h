/**
 * Signal.h
 * Author: Daniel Karrels dan@karrels.com
 * Copyright (C) 2003 Daniel Karrels <dan@karrels.com>
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
 * $Id: Signal.h,v 1.5 2003/12/17 18:21:36 dan_karrels Exp $
 */

#ifndef __SIGNAL_H
#define __SIGNAL_H "$Id: Signal.h,v 1.5 2003/12/17 18:21:36 dan_karrels Exp $"

#include	<pthread.h>

namespace gnuworld
{

/**
 * A class used to safely handle asynchronous (non-realtime) signals.
 * It uses a nonblocking pipe (rather than other solutions, see
 * my thesis) to solve the multiple consumer, single nonblocking
 * producer p/c problem.
 */
class Signal
{

protected:

	/**
	 * This variable is true if there exists an uncoverable error.
	 */
	static bool		signalError ;

	/**
	 * The FD for the read side of the pipe.
	 */
	static int		readFD ;

	/**
	 * The FD for the write side of the pipe.
	 */
	static int		writeFD ;

	/**
	 * A mutex to guard access to the Singleton.
	 */
	static pthread_mutex_t	singletonMutex ;

	/**
	 * The Singleton instance.
	 */
	static Signal*		theInstance ;

	/**
	 * This mutex guards from multiple threads performing a get()
	 */
	static pthread_mutex_t	pipeMutex ;

public:
	/**
	 * Release resources associated with this class.
	 */
	virtual ~Signal() ;

	/**
	 * Retrieve the Singleton instance of this class, creating
	 * it if necessary.
	 * Once an instance is created, all signals currently
	 * supported by this class will be remapped to this subsystem.
	 */
	static Signal*		getInstance() ;

	/**
	 * Add a signal to the signal queue.
	 * This method is meant to be called *only* from the
	 * asynchronous signal handler.
	 */
	static void		AddSignal( int whichSig ) ;

	/**
	 * The semantics of the return statement are a little backwards here:
	 * - true indicates that the caller should check the value of
	 *   theSignal for either a new signal or an error state (-1)
	 * - false indicates that no error and no signal are ready
	 */
	static bool		getSignal( int& theSignal ) ;

	/**
	 * This method will return true if there is a non-recoverable
	 * error in the signal handler subsystem.
	 */
	static bool		isError() ;

private:
	/**
	 * Private constructor, make this class a Singleton.
	 */
	Signal() ;

protected:
	/**
	 * Convenience method to close the pipes when a critical
	 * error occurs.  In this case, signalError will remain
	 * set, and both pipes are invalid, no signal delivery
	 * will occur.
	 */
	static void		closePipes() ;

	/**
	 * Opens both pipes, and configures in nonblocking mode.
	 */
	static bool		openPipes() ;

} ;

} // namespace gnuworld

#endif // __SIGNAL_H
