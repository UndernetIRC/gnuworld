/**
 * Logger.cc
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

#include <cstdarg>
#include <cstdio>

#include "gnuworld_config.h"
#include "Logger.h"

namespace gnuworld {

namespace logging {

/** Initialise our static member */
Logger* Logger::theLogger = 0;

/*********************************
 ** S T A T I C   M E T H O D S **
 *********************************/

/**
 * Implementation of Logger as a singleton.
 */
Logger* Logger::getInstance()
{
  if(!theLogger) theLogger = new Logger;

  return theLogger;
}


/**
 * Allow registration of a new log target
 */
void Logger::addLogger(logTarget* theTarget)
{
  logTargets.push_back(theTarget);
}


/**
 * Allow a client to log an event type
 */
void Logger::log(const events::eventType& theEvent, const string& theMessage)
{
  for(logTargetsType::iterator ptr = logTargets.begin(); ptr != logTargets.end(); ptr++) {
    (*ptr)->log(theEvent, theMessage);
  }
}


/**
 * Allow a client to log an event.
 * This is a convenience wrapper around log(events::eventType&, const string&).
 */
void Logger::log(const events::eventType& theEvent, const char* format, ... ) {
  char buffer[512];

  va_list _list;

  va_start(_list, format);
  ::vsnprintf(buffer, 512, format, _list);
  va_end(_list);

  log(theEvent, string(buffer));
}


/**
 * Empty constructor
 */
Logger::Logger()
{
}

/**
 * Destructor
 */
Logger::~Logger()
{
}

} // namespace logging

} // namespace gnuworld

