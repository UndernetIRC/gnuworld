#ifndef _LOGTARGET_H
#define _LOGTARGET_H "$Id: logTarget.h,v 1.3 2002/09/05 16:02:43 dan_karrels Exp $"

/*
 * logTarget.h
 *
 * Declaration of abstract base class logTarget
 *
 * Any classes wishing to be a target for logging must
 * inherit this.
 */

#include <string>

using std::string ;

namespace gnuworld {

namespace logging {

namespace events {

typedef int eventType;

static const eventType E_PROTOCOL = 0x0001;
static const eventType E_SQL      = 0x0002;
static const eventType E_DEBUG    = 0x0004;
static const eventType E_INFO     = 0x0008;
static const eventType E_WARNING  = 0x0010;
static const eventType E_ERROR    = 0x0020;

} // namespace events

class logTarget {
  public:
    virtual void log(const events::eventType&, const string&) = 0;
}; // class logTarget

} // namespace logging

} // namespace gnuworld

#endif
