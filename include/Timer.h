/**
 * Timer.h
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
 */

#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <time.h>

namespace gnuworld {

class Timer {
public:
	inline Timer(bool autostart = true)
		{ if(autostart) { Start(); } }

	inline void Start()
		{ gettimeofday(&startTime, 0); }
	inline void Stop()
		{ gettimeofday(&stopTime, 0); }
	inline unsigned int getTimeMS()
		{ return	(stopTime.tv_sec - startTime.tv_sec) * 1000 +
				(stopTime.tv_usec - startTime.tv_usec) / 1000; }
	inline unsigned long getTimeUS()
		{ return	(stopTime.tv_sec - startTime.tv_sec) * 1000000 +
				(stopTime.tv_usec - startTime.tv_usec); }
	inline unsigned int stopTimeMS()
		{ Stop(); return getTimeMS(); }
	inline unsigned long stopTimeUS()
		{ Stop(); return getTimeUS(); }
protected:
	struct timeval startTime, stopTime;
}; // class Timer

} // namespace gnuworld

#endif
