#ifndef _LOGGER_H
#define _LOGGER_H "$Id: Logger.h,v 1.2 2002/09/05 16:02:39 dan_karrels Exp $"

#include <string>
#include <vector>

#include "logTarget.h"

using std::vector ;
using std::string ;

namespace gnuworld {

namespace logging {

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
