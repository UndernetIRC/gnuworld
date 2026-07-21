/**
 * sqlSpamAction.h
 *
 * Represents a row in the spam_actions table.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef SQLSPAMACTION_H
#define SQLSPAMACTION_H

#include <string>
#include "dbHandle.h"

namespace gnuworld {
namespace ds {

using std::string;

class sqlSpamAction {
  public:
    sqlSpamAction(dbHandle*);
    virtual ~sqlSpamAction();

    /* Accessors */
    inline int           getId()          const { return id; }
    inline const string& getName()        const { return name; }
    inline const string& getActionType() const { return action_type; }
    inline int           getDuration()   const { return duration; }
    inline const string& getReason()     const { return reason; }
    inline int           getDelay()      const { return delay; }
    inline int           getRandMin()    const { return rand_min; }
    inline int           getRandMax()    const { return rand_max; }
    inline bool          isEnabled()     const { return enabled; }
    inline bool          isPrefixAuto()  const { return prefix_auto; }
    inline int           getCreatedTs()  const { return created_ts; }
    inline int           getModifiedTs() const { return modified_ts; }
    inline int           getModifiedBy() const { return modified_by; }

    /* Mutators */
    inline void setId(int v)                    { id = v; }
    inline void setName(const string& v)        { name = v; }
    inline void setActionType(const string& v)  { action_type = v; }
    inline void setDuration(int v)              { duration = v; }
    inline void setReason(const string& v)      { reason = v; }
    inline void setDelay(int v)                 { delay = v; }
    inline void setRandMin(int v)               { rand_min = v; }
    inline void setRandMax(int v)               { rand_max = v; }
    inline void setEnabled(bool v)              { enabled = v; }
    inline void setPrefixAuto(bool v)           { prefix_auto = v; }
    inline void setCreatedTs(int v)             { created_ts = v; }
    inline void setModifiedTs(int v)            { modified_ts = v; }
    inline void setModifiedBy(int v)            { modified_by = v; }

    void setAllMembers(int row);
    bool commit();
    bool insert();
    bool remove();

  protected:
    int    id;
    string name;
    string action_type;
    int    duration;   // -1 means NULL
    string reason;
    int    delay;
    int    rand_min;   // -1 means NULL
    int    rand_max;   // -1 means NULL
    bool   enabled;
    bool   prefix_auto; // whether GLINE reasons get "AUTO " ahead of "[N] "
    int    created_ts;
    int    modified_ts;
    int    modified_by; // 0 means NULL

    dbHandle* SQLDb;
};

} // namespace ds
} // namespace gnuworld

#endif
