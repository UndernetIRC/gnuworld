/**
 * TimerHandler.h
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
 * $Id: TimerHandler.h,v 1.6 2004/05/19 19:46:33 jeekay Exp $
 */

#ifndef __TIMERHANDLER_H
#define __TIMERHANDLER_H "$Id: TimerHandler.h,v 1.6 2004/05/19 19:46:33 jeekay Exp $"

namespace gnuworld
{

/**
 * This is the abstract base class used in the GNUWorld timer system.
 * All timer handlers must subclass this class, and define the
 * abstract method OnTimer().  The OnTimer() method will be called
 * by the timer system when the timer has expired.
 */
class TimerHandler
{

public:

	/**
	 * The constructor does nothing.
	 */
	TimerHandler() {}

	/**
	 * The destructor does nothing.
	 */
	virtual ~TimerHandler() {}

	/**
	 * The type used to represent timer events.
	 */
	typedef unsigned int timerID ;

	/**
	 * Handle a timer event.  The first argument is the
	 * handle for the timer registration, and the second
	 * is the argument that was passed when registering the
	 * timer.
	 */
	virtual void OnTimer( const timerID& , void* ) = 0 ;

	/**
	 * This method is invoked when the server removes a
	 * timed event without being requested by the TimerHandler.
	 * This can happen when the TimerHandler is being destroyed,
	 * but has not removed all of its timers yet.
	 */
	virtual void OnTimerDestroy( timerID, void * ) {}

} ;

/**
 * This macro is meant to assist in creating new subclasses of
 * the TimerHandler class.
 */
#define SUBCLASS_TIMERHANDLER(className) \
class className##Timer : public TimerHandler \
{ \
public: \
className##Timer() {} \
virtual ~className##Timer() {} \
virtual void OnTimer( timerID, void* ) ; \
} ;

} // namespace gnuworld

#endif // __TIMERHANDLER_H
