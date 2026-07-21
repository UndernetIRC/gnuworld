/**
 * sqlSpamRuleAction.h
 *
 * Represents a row in the spam_rule_actions table.
 * Each row is an independent binding of an action to a rule,
 * with optional per-rule overrides.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef SQLSPAMRULEACTION_H
#define SQLSPAMRULEACTION_H

#include <string>
#include "dbHandle.h"

namespace gnuworld {
namespace ds {

using std::string;

class sqlSpamAction;

class sqlSpamRuleAction {
  public:
    sqlSpamRuleAction(dbHandle*);
    virtual ~sqlSpamRuleAction();

    /* Accessors */
    inline int           getId()                     const { return id; }
    inline int           getRuleId()                 const { return rule_id; }
    inline int           getActionId()               const { return action_id; }
    inline const string& getActionType()             const { return action_type; }
    // -1 means NULL (use spam_actions.duration)
    inline int           getActionDurationOverride() const { return action_duration_override; }
    // empty string means NULL (use spam_actions.reason)
    inline const string& getActionReasonOverride()   const { return action_reason_override; }
    // -1 means NULL (use spam_actions.delay)
    inline int           getDelayOverride()          const { return delay_override; }

    /* Mutators */
    inline void setId(int v)                              { id = v; }
    inline void setRuleId(int v)                          { rule_id = v; }
    inline void setActionId(int v)                        { action_id = v; }
    inline void setActionType(const string& v)            { action_type = v; }
    inline void setActionDurationOverride(int v)          { action_duration_override = v; }
    inline void setActionReasonOverride(const string& v)  { action_reason_override = v; }
    inline void setDelayOverride(int v)                   { delay_override = v; }

    void setAllMembers(int row);
    bool insert();
    bool remove();

    /* Resolved sqlSpamAction* for action_id, set by
     * dronescan::relinkSpamGraph(). Not persisted. */
    inline sqlSpamAction* getAction() const       { return actionPtr; }
    inline void setAction(sqlSpamAction* a)       { actionPtr = a; }

  protected:
    int    id;
    int    rule_id;
    int    action_id;
    string action_type;
    int    action_duration_override; // -1 = NULL
    string action_reason_override;   // empty = NULL
    int    delay_override;           // -1 = NULL

    dbHandle* SQLDb;

    // In-memory link, not persisted; see getAction().
    sqlSpamAction* actionPtr;
};

} // namespace ds
} // namespace gnuworld

#endif
