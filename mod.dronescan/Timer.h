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
 *
 * $Id: Timer.h,v 1.2 2003/06/19 22:58:30 dan_karrels Exp $
 */

#ifndef TIMER_H
#define TIMER_H "$Id: Timer.h,v 1.2 2003/06/19 22:58:30 dan_karrels Exp $"

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
