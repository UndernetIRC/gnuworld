#ifndef _LOGGER_H
#define _LOGGER_H "$Id: Logger.h,v 1.3 2002/11/25 03:56:15 jeekay Exp $"

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
