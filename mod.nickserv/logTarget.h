#ifndef _LOGTARGET_H
#define _LOGTARGET_H "$Id: logTarget.h,v 1.4 2002/11/25 03:56:15 jeekay Exp $"

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

using std::string ;

namespace events {

typedef int eventType;

static const eventType E_MIN      = 0x0000;
static const eventType E_MAX      = 0x003F;

/* Make sure any new additions here are also added to logTarget::getIdent */
static const eventType E_PROTOCOL = 0x0001;
static const eventType E_SQL      = 0x0002;
static const eventType E_DEBUG    = 0x0004;
static const eventType E_INFO     = 0x0008;
static const eventType E_WARNING  = 0x0010;
static const eventType E_ERROR    = 0x0020;

} // namespace events

class logTarget {
  public:
    inline static string getIdent(const events::eventType& theEvent) {
      string temp;
      if(theEvent & events::E_ERROR)    temp += 'E';
      if(theEvent & events::E_WARNING)  temp += 'W';
      if(theEvent & events::E_INFO)     temp += 'I';
      if(theEvent & events::E_DEBUG)    temp += 'D';
      if(theEvent & events::E_SQL)      temp += 'S';
      if(theEvent & events::E_PROTOCOL) temp += 'P';
      return temp;
    }

    virtual void log(const events::eventType&, const string&) = 0;
}; // class logTarget

} // namespace logging

} // namespace gnuworld

#endif
