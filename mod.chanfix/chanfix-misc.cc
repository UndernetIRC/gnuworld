/**
 * chanfix-misc.cc
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

#include <string>
#include <sstream>

#include <cstdarg>

#include "Network.h"

#include "chanfix.h"

namespace gnuworld {

namespace chanfix {

using std::endl;

void chanfix::log(const logging::loglevel& level, const char* format, ... )
{
	char buf[1024] = {0};
	va_list list;

	va_start(list, format);
	vsnprintf(buf, 1024, format, list);
	va_end(list);

	log(level, std::string(buf));
}


void chanfix::log(const logging::loglevel& level,
	const std::string& message)
{
	if( 0 == confLogLevel & level ) { return; }

	/* Check our logging channel exists */
	Channel *logChannel = Network->findChannel(confConsoleChannel);

	if( !logChannel ) {
		elog	<< "chanfix> ERROR: Unable to locate logging channel."
			<< endl;
		return;
	}

	Message(logChannel, message.c_str());
}


void chanfix::setConsoleTopic()
{
	char buffer[512];
	sprintf(buffer, "Giving %u points per %us",
		confPointsAuth,
		confPeriod
		);

	std::stringstream newTopic;
	newTopic	<< this->getCharYYXXX()
			<< " T "
			<< confConsoleChannel
			<< " :"
			<< buffer
			<< endl;

	this->Write(newTopic.str());
}

} // namespace chanfix

} // namespace gnuworld
