#include <new>

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

