/* TimerHandler.h */

#ifndef __TIMERHANDLER_H
#define __TIMERHANDLER_H "$Id: TimerHandler.h,v 1.2 2001/05/17 21:32:57 dan_karrels Exp $"

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
	virtual int OnTimer( timerID, void* ) = 0 ;

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
virtual int OnTimer( timerID, void* ) ; \
} ;

} // namespace gnuworld

#endif // __TIMERHANDLER_H
