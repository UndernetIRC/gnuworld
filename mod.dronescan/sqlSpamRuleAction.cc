/**
 * sqlSpamRuleAction.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <sstream>
#include "dbHandle.h"
#include "ELog.h"
#include "sqlSpamRuleAction.h"
#include "misc.h"

namespace gnuworld {
namespace ds {

using std::endl;
using std::stringstream;

sqlSpamRuleAction::sqlSpamRuleAction(dbHandle* _SQLDb)
    : id(0), rule_id(0), action_id(0), action_type(),
      action_duration_override(-1), action_reason_override(),
      delay_override(-1),
      SQLDb(_SQLDb), actionPtr(nullptr)
{}

sqlSpamRuleAction::~sqlSpamRuleAction() {}

// Column order matches the SELECT in preloadSpamRuleActions():
// id, rule_id, action_id, action_type,
// action_duration_override, action_reason_override, delay_override
void sqlSpamRuleAction::setAllMembers(int row)
{
    id          = atoi(SQLDb->GetValue(row, 0).c_str());
    rule_id     = atoi(SQLDb->GetValue(row, 1).c_str());
    action_id   = atoi(SQLDb->GetValue(row, 2).c_str());
    action_type = SQLDb->GetValue(row, 3);
    const string dur = SQLDb->GetValue(row, 4);
    action_duration_override = !dur.empty() ? atoi(dur.c_str()) : -1;
    const string rea = SQLDb->GetValue(row, 5);
    action_reason_override   = !rea.empty() ? rea : "";
    const string del = SQLDb->GetValue(row, 6);
    delay_override = !del.empty() ? atoi(del.c_str()) : -1;
}

bool sqlSpamRuleAction::insert()
{
    stringstream q;
    q << "INSERT INTO spam_rule_actions "
      << "(rule_id, action_id, action_type, "
      << "action_duration_override, action_reason_override, delay_override) VALUES ("
      << rule_id   << ", "
      << action_id << ", "
      << "'" << escapeSQLChars(action_type) << "', "
      << (action_duration_override >= 0 ? std::to_string(action_duration_override) : "NULL") << ", "
      << (action_reason_override.empty() ? "NULL" : ("'" + escapeSQLChars(action_reason_override) + "'")) << ", "
      << (delay_override >= 0 ? std::to_string(delay_override) : "NULL")
      << ") RETURNING id";

    if (!SQLDb->Exec(q, true)) {
        elog << "sqlSpamRuleAction::insert> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    if (SQLDb->Tuples() > 0)
        id = atoi(SQLDb->GetValue(0, 0));
    return true;
}

bool sqlSpamRuleAction::remove()
{
    stringstream q;
    q << "DELETE FROM spam_rule_actions WHERE id = " << id;
    if (!SQLDb->Exec(q)) {
        elog << "sqlSpamRuleAction::remove> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

} // namespace ds
} // namespace gnuworld
