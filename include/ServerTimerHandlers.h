/* ServerTimerHandlers.h */

#ifndef __SERVERTIMERHANDLERS_H
#define __SERVERTIMERHANDLERS_H "$Id: ServerTimerHandlers.h,v 1.3 2001/05/17 21:32:57 dan_karrels Exp $"

#include	"TimerHandler.h"

#include	<ctime>

namespace gnuworld
{

/// Forward declaration of the xServer class
class xServer ;

/**
 * This is the abstract base class from which each timer to be used
 * by the GNUWorld server core must derive.  Each server timer
 * must inherit from this class, and define the abstract method
 * OnTimer().  The OnTimer() method will be called by the timer
 * system when the timer expires.
 */
class ServerTimerHandler : public TimerHandler
{
public:
	/**
	 * The constructor receives a pointer to the server core,
	 * and the interval at which to reschedule the timer,
	 * if the timer handler chooses to do so.
	 */
	ServerTimerHandler( xServer* _theServer, time_t _updateInterval )
	: theServer( _theServer ),
	  updateInterval( _updateInterval )
	{}

	/**
	 * This is the virtual destructor, which currently does
	 * nothing, but must be present in a class hierarchy.
	 */
	virtual ~ServerTimerHandler()
	{}

	/**
	 * This method is called when the timer expires.  The timerID
	 * argument is registration ID of the timer.  The void*
	 * argument is a pointer to whatever data was given during
	 * timer registration.  Make sure that this data is placed
	 * into persistent memory, otherwise a segmentation fault
	 * may occur when running OnTimer().
	 */
	virtual int OnTimer( timerID, void* ) = 0 ;

protected:

	/**
	 * This is a pointer to the server core.
	 */
	xServer*	theServer ;

	/**
	 * This is the timer rescheduling interval.
	 */
	time_t		updateInterval ;
} ;

/**
 * This macro is used to create a generic subclass of class
 * ServerTimerHandler, and to declare the OnTimer() method as
 * concrete.
 */
#define SUBCLASS_SERVERTIMERHANDLER(className)\
class className##Timer : public ServerTimerHandler \
{ \
public: \
	className##Timer( xServer* theServer, time_t updateInterval ) \
	: ServerTimerHandler( theServer, updateInterval ) \
	{} \
	virtual ~className##Timer() {} \
	virtual int OnTimer( timerID, void* ) ; \
} ;

SUBCLASS_SERVERTIMERHANDLER( GlineUpdate )
SUBCLASS_SERVERTIMERHANDLER( PING )

} // namespace gnuworld

#endif // __SERVERTIMERHANDLER_H
