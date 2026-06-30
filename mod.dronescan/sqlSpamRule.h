/**
 * sqlSpamRule.h
 *
 * Represents a row in the spam_rules table.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef SQLSPAMRULE_H
#define SQLSPAMRULE_H

#include <string>
#include "dbHandle.h"

namespace gnuworld {
namespace ds {

using std::string;

class sqlSpamRule {
  public:
    sqlSpamRule(dbHandle*);
    virtual ~sqlSpamRule();

    /* Accessors */
    inline int           getId()               const { return id; }
    inline const string& getName()             const { return name; }
    inline const string& getDescription()      const { return description; }
    inline int           getThreshold()        const { return threshold; }
    inline int           getWaitOnRuleId()     const { return wait_on_rule_id; }
    inline bool          isAllChans()          const { return allchans; }
    inline const string& getPointsPerOverride()const { return points_per_override; }
    inline bool          isEnabled()           const { return enabled; }
    inline int           getCreatedTs()        const { return created_ts; }
    inline int           getModifiedTs()       const { return modified_ts; }
    inline int           getModifiedBy()       const { return modified_by; }

    /* Mutators */
    inline void setId(int v)                         { id = v; }
    inline void setName(const string& v)             { name = v; }
    inline void setDescription(const string& v)      { description = v; }
    inline void setThreshold(int v)                  { threshold = v; }
    inline void setWaitOnRuleId(int v)               { wait_on_rule_id = v; }
    inline void setAllChans(bool v)                  { allchans = v; }
    inline void setPointsPerOverride(const string& v){ points_per_override = v; }
    inline void setEnabled(bool v)                   { enabled = v; }
    inline void setCreatedTs(int v)                  { created_ts = v; }
    inline void setModifiedTs(int v)                 { modified_ts = v; }
    inline void setModifiedBy(int v)                 { modified_by = v; }

    void setAllMembers(int row);
    bool commit();
    bool insert();
    bool remove();

  protected:
    int    id;
    string name;
    string description;
    int    threshold;
    int    wait_on_rule_id;      // 0 means NULL / no dependency
    // true = all monitored channels (spam_rule_channels = exclusion list)
    // false = only channels in spam_rule_channels (inclusion list)
    bool   allchans;
    string points_per_override;  // empty string means NULL (use per-event default)
    bool   enabled;
    int    created_ts;
    int    modified_ts;
    int    modified_by;          // 0 means NULL

    dbHandle* SQLDb;
};

} // namespace ds
} // namespace gnuworld

#endif
