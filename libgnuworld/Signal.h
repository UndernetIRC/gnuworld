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
 * $Id: Signal.h,v 1.2 2003/06/17 15:13:53 dan_karrels Exp $
 */

#ifndef __SIGNAL_H
#define __SIGNAL_H "$Id: Signal.h,v 1.2 2003/06/17 15:13:53 dan_karrels Exp $"

#include	<pthread.h>

#include	<queue>

namespace gnuworld
{
using std::queue ;

class Signal
{

protected:
	static queue< int >	signals1 ;
	static queue< int >	signals2 ;

	static pthread_mutex_t	signals1Mutex ;

public:
	Signal() ;
	virtual ~Signal() ;

	/**
	 * Add a signal to the signal queue.
	 * This method is meant to be called *only* from the
	 * asynchronous signal handler.
	 */
	static void		AddSignal( int whichSig ) ;

	/**
	 * This method returns the oldest signal in the queue.
	 * This method must be called by only one thread, and never
	 * by the signal handler thread.
	 */
	static int		getSignal() ;

} ;

} // namespace gnuworld

#endif // __SIGNAL_H
