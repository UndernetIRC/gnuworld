/* ServerTimerHandlers.h */

#ifndef __SERVERTIMERHANDLERS_H
#define __SERVERTIMERHANDLERS_H "$Id: ServerTimerHandlers.h,v 1.2 2001/02/05 18:58:12 dan_karrels Exp $"

#include	"TimerHandler.h"

namespace gnuworld
{

SUBCLASS_TIMERHANDLER( GlineUpdate )
SUBCLASS_TIMERHANDLER( PING )

} // namespace gnuworld

#endif // __SERVERTIMERHANDLER_H
