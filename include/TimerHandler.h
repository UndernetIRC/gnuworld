/* TimerHandler.h */

#ifndef __TIMERHANDLER_H
#define __TIMERHANDLER_H "$Id: TimerHandler.h,v 1.1 2001/01/28 19:27:35 dan_karrels Exp $"

namespace gnuworld
{

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
	 * The type used to represent client timer events.
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
