/**
 * chanfix-xclient.cc
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
 * $Id: chanfix-xclient.cc,v 1.3 2004/05/25 21:17:53 jeekay Exp $
 */

#include "chanfix.h"

namespace gnuworld {

namespace chanfix {

void chanfix::OnAttach()
{
	xClient::OnAttach();

	/* Lastly, kick off any timers we might need */
	time_t next;

	next = ::time(0) + confStartDelay;
	timerCount = MyUplink->RegisterTimer(next, this, 0);
}


void chanfix::BurstChannels()
{
	xClient::BurstChannels();

	MyUplink->JoinChannel(this, confConsoleChannel, confConsoleModes);
}


void chanfix::OnTimer( const TimerHandler::timerID& theTimer , void* _data )
{
	xClient::OnTimer( theTimer , _data );

	time_t next = ::time(0);

	if( theTimer == timerCount ) {
		log(logging::DEBUG, "Entering count cycle");

		doCountUpdate();

		next += confPeriod;

		timerCount = MyUplink->RegisterTimer(next, this, 0);
	} else {
		assert(0);
	}
}

} // namespace chanfix

} // namespace gnuworld
