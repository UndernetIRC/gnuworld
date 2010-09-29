/**
 * Logger.h
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

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>

#include "logTarget.h"

namespace gnuworld {

namespace logging {

using std::string ;
using std::vector ;

class Logger {

  public:
    /**
     * Implement Logger as a singleton. This method is the only way to get
     * a reference to a Logger instance.
     */
    static Logger* getInstance();

    /**
     * Allow registration of a new logTarget
     */
    void addLogger(logTarget*);

    /**
     * Allow logging of messages
     */
    void log(const events::eventType&, const string&);
    void log(const events::eventType&, const char*, ... );

    /**
     * Type to contain logTargets
     */
    typedef vector< logTarget* > logTargetsType;

    /**
     * Container for logTargets
     */
    logTargetsType logTargets;

  protected:
    /** Empty constructor */
    Logger();

    /** Empty destructor */
    ~Logger();

    /** Current instance of Logger */
    static Logger* theLogger;

}; // class Logger

} // namespace logging

} // namespace gnuworld

#endif
