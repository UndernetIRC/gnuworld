#ifndef _LOGTARGET_H
#define _LOGTARGET_H "$Id: logTarget.h,v 1.1 2002/08/27 16:10:52 jeekay Exp $"

/*
 * logTarget.h
 *
 * Declaration of abstract base class logTarget
 *
 * Any classes wishing to be a target for logging must
 * inherit this.
 */

#include <string>

namespace gnuworld {

namespace logging {

namespace events {

typedef int eventType;

static const eventType E_DEBUG = 0x0001;

} // namespace events

class logTarget {
  public:
    virtual void log(const events::eventType&, const string&) = 0;
}; // class logTarget

} // namespace log

} // namespace log

#endif
