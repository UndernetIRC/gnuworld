#ifndef TIMER_H
#define TIMER_H "$Id: Timer.h,v 1.1 2003/05/04 12:41:14 jeekay Exp $"

#include <sys/time.h>

namespace gnuworld {

class Timer {
public:
	inline void Start()
		{ gettimeofday(&startTime, 0); }
	inline void Stop()
		{ gettimeofday(&stopTime, 0); }
	inline unsigned int getTimeMS()
		{ return	(stopTime.tv_sec - startTime.tv_sec) * 1000 +
				(stopTime.tv_usec - startTime.tv_usec) / 1000; }
	inline unsigned int stopTimeMS()
		{ Stop(); return getTimeMS(); }
protected:
	struct timeval startTime, stopTime;
}; // class Timer

} // namespace gnuworld

#endif
