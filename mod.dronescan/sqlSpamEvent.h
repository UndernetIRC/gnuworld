/**
 * sqlSpamEvent.h
 *
 * Represents a row in the spam_events table.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef SQLSPAMEVENT_H
#define SQLSPAMEVENT_H

#include <string>
#include "dbHandle.h"

namespace gnuworld {
namespace ds {

using std::string;

class sqlSpamEvent {
  public:
    sqlSpamEvent(dbHandle*);
    virtual ~sqlSpamEvent();

    /* Accessors */
    inline int           getId()               const { return id; }
    inline const string& getName()             const { return name; }
    inline const string& getDescription()      const { return description; }
    inline const string& getEventType()        const { return event_type; }
    inline const string& getEventParam()       const { return event_param; }
    // target: bitmask ? CHAN=1, PRIVMSG=2, NOTICE=4, PART=8, QUIT=16, ALL=31
    inline int           getTarget()           const { return target; }
    inline bool          isCaseSensitive()     const { return case_sensitive; }
    inline int           getPoints()           const { return points; }
    inline int           getPointExpiry()      const { return point_expiry; }
    inline int           getMaxOccurrence()    const { return max_occurrence; }
    inline int           getRequiresEventId()  const { return requires_event_id; }
    inline bool          isEnabled()           const { return enabled; }
    inline bool          isRepeatCrossUser()   const { return repeat_crossuser; }
    inline int           getRepeatMinCount()   const { return repeat_min_count; }
    inline const string& getRepeatExclusionRegex() const { return repeat_exclusion_regex; }
    inline int           getCreatedTs()        const { return created_ts; }
    inline int           getModifiedTs()       const { return modified_ts; }
    inline int           getModifiedBy()       const { return modified_by; }

    /* Mutators */
    inline void setId(int v)                          { id = v; }
    inline void setName(const string& v)              { name = v; }
    inline void setDescription(const string& v)       { description = v; }
    inline void setEventType(const string& v)         { event_type = v; }
    inline void setEventParam(const string& v)        { event_param = v; }
    inline void setTarget(int v)                      { target = v; }
    inline void setCaseSensitive(bool v)              { case_sensitive = v; }
    inline void setPoints(int v)                      { points = v; }
    inline void setPointExpiry(int v)                 { point_expiry = v; }
    inline void setMaxOccurrence(int v)               { max_occurrence = v; }
    inline void setRequiresEventId(int v)             { requires_event_id = v; }
    inline void setEnabled(bool v)                    { enabled = v; }
    inline void setRepeatCrossUser(bool v)            { repeat_crossuser = v; }
    inline void setRepeatMinCount(int v)              { repeat_min_count = v; }
    inline void setRepeatExclusionRegex(const string& v) { repeat_exclusion_regex = v; }
    inline void setCreatedTs(int v)                   { created_ts = v; }
    inline void setModifiedTs(int v)                  { modified_ts = v; }
    inline void setModifiedBy(int v)                  { modified_by = v; }

    void setAllMembers(int row);
    bool commit();
    bool insert();
    bool remove();

  protected:
    int    id;
    string name;
    string description;
    string event_type;
    string event_param;
    int    target;               // bitmask: CHAN=1, PRIVMSG=2, NOTICE=4, PART=8, QUIT=16
    bool   case_sensitive;
    int    points;
    int    point_expiry;
    int    max_occurrence;       // -1 means NULL / unlimited
    int    requires_event_id;    // 0 means NULL / no dependency
    bool   enabled;
    // TEXT_REPEAT-specific fields
    bool   repeat_crossuser;
    int    repeat_min_count;
    string repeat_exclusion_regex;
    int    created_ts;
    int    modified_ts;
    int    modified_by;          // 0 means NULL

    dbHandle* SQLDb;
};

} // namespace ds
} // namespace gnuworld

#endif
