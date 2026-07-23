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
#include <vector>
#include "dbHandle.h"

namespace gnuworld {
namespace ds {

using std::string;

class sqlSpamEvent;
class sqlSpamRuleAction;

// One event linked to a rule via spam_rule_events, plus that binding's
// points_override (-1 = use the event's own points).
struct RuleEventLink {
    sqlSpamEvent* event;
    int           pointsOverride;
    RuleEventLink(sqlSpamEvent* e, int po) : event(e), pointsOverride(po) {}
};

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
    inline const string& getPointsPer()        const { return points_per; }
    inline bool          isScoreGlobally()     const { return score_globally; }
    inline bool          isEnabled()           const { return enabled; }
    inline bool          isSilent()            const { return silent; }
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
    inline void setPointsPer(const string& v)        { points_per = v; }
    inline void setScoreGlobally(bool v)             { score_globally = v; }
    inline void setEnabled(bool v)                   { enabled = v; }
    inline void setSilent(bool v)                    { silent = v; }
    inline void setCreatedTs(int v)                  { created_ts = v; }
    inline void setModifiedTs(int v)                 { modified_ts = v; }
    inline void setModifiedBy(int v)                 { modified_by = v; }

    void setAllMembers(int row);
    bool commit();
    bool insert();
    bool remove();

    /* Events and actions linked to this rule via spam_rule_events /
     * spam_rule_actions, resolved by dronescan::relinkSpamGraph(). Not
     * filtered by enabled state - callers check isEnabled() themselves. */
    inline const std::vector<RuleEventLink>& getEvents() const  { return linkedEvents; }
    inline void clearEvents()                                   { linkedEvents.clear(); }
    inline void addEvent(sqlSpamEvent* e, int pointsOverride)    { linkedEvents.push_back(RuleEventLink(e, pointsOverride)); }

    inline const std::vector<sqlSpamRuleAction*>& getActions() const { return linkedActions; }
    inline void clearActions()                                  { linkedActions.clear(); }
    inline void addAction(sqlSpamRuleAction* a)                 { linkedActions.push_back(a); }

  protected:
    int    id;
    string name;
    string description;
    int    threshold;
    int    wait_on_rule_id;      // 0 means NULL / no dependency
    // true = all monitored channels (spam_rule_channels = exclusion list)
    // false = only channels in spam_rule_channels (inclusion list)
    bool   allchans;
    string points_per;           // "CLIENT" or "IP"
    bool   score_globally;
    bool   enabled;
    bool   silent;                // suppress report-only console line
    int    created_ts;
    int    modified_ts;
    int    modified_by;          // 0 means NULL

    dbHandle* SQLDb;

    // In-memory link graph, not persisted; see getEvents()/getActions().
    std::vector<RuleEventLink>      linkedEvents;
    std::vector<sqlSpamRuleAction*> linkedActions;
};

} // namespace ds
} // namespace gnuworld

#endif
