/**
 * sqlSpamAction.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <sstream>
#include "dbHandle.h"
#include "ELog.h"
#include "sqlSpamAction.h"
#include "misc.h"

namespace gnuworld {
namespace ds {

using std::endl;
using std::stringstream;

sqlSpamAction::sqlSpamAction(dbHandle* _SQLDb)
    : id(0), name(), action_type(), duration(3600), reason(),
      delay(0), rand_min(-1), rand_max(-1), enabled(true), prefix_auto(true),
      created_ts(0), modified_ts(0), modified_by(0),
      SQLDb(_SQLDb)
{}

sqlSpamAction::~sqlSpamAction() {}

// Column order matches the SELECT in preloadSpamActions():
// id, name, action_type, duration, reason, delay, rand_min, rand_max,
// enabled, prefix_auto, created_ts, modified_ts, modified_by
void sqlSpamAction::setAllMembers(int row)
{
    id          = atoi(SQLDb->GetValue(row, 0).c_str());
    name        = SQLDb->GetValue(row, 1);
    action_type = SQLDb->GetValue(row, 2);
    const string dur = SQLDb->GetValue(row, 3);
    duration    = !dur.empty() ? atoi(dur.c_str()) : -1;
    reason      = SQLDb->GetValue(row, 4);
    delay       = atoi(SQLDb->GetValue(row, 5).c_str());
    const string rmin = SQLDb->GetValue(row, 6);
    rand_min    = !rmin.empty() ? atoi(rmin.c_str()) : -1;
    const string rmax = SQLDb->GetValue(row, 7);
    rand_max    = !rmax.empty() ? atoi(rmax.c_str()) : -1;
    enabled     = (SQLDb->GetValue(row, 8) == "t");
    prefix_auto = (SQLDb->GetValue(row, 9) == "t");
    created_ts  = atoi(SQLDb->GetValue(row, 10).c_str());
    modified_ts = atoi(SQLDb->GetValue(row, 11).c_str());
    const string modBy = SQLDb->GetValue(row, 12);
    modified_by = !modBy.empty() ? atoi(modBy.c_str()) : 0;
}

bool sqlSpamAction::commit()
{
    stringstream q;
    q << "UPDATE spam_actions SET "
      << "name = '"        << escapeSQLChars(name)        << "', "
      << "action_type = '" << escapeSQLChars(action_type) << "', "
      << "duration = "     << (duration >= 0 ? std::to_string(duration) : "NULL") << ", "
      << "reason = "       << (reason.empty() ? "NULL" : ("'" + escapeSQLChars(reason) + "'")) << ", "
      << "delay = "        << delay << ", "
      << "rand_min = "     << (rand_min >= 0 ? std::to_string(rand_min) : "NULL") << ", "
      << "rand_max = "     << (rand_max >= 0 ? std::to_string(rand_max) : "NULL") << ", "
      << "enabled = "      << (enabled ? "true" : "false") << ", "
      << "prefix_auto = "  << (prefix_auto ? "true" : "false") << ", "
      << "modified_ts = "  << modified_ts << ", "
      << "modified_by = "  << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << " WHERE id = "    << id;

    if (!SQLDb->Exec(q)) {
        elog << "sqlSpamAction::commit> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

bool sqlSpamAction::insert()
{
    stringstream q;
    q << "INSERT INTO spam_actions "
      << "(name, action_type, duration, reason, delay, rand_min, rand_max, "
      << "enabled, prefix_auto, created_ts, modified_ts, modified_by) VALUES ("
      << "'"  << escapeSQLChars(name)        << "', "
      << "'"  << escapeSQLChars(action_type) << "', "
      << (duration >= 0 ? std::to_string(duration) : "NULL") << ", "
      << (reason.empty() ? "NULL" : ("'" + escapeSQLChars(reason) + "'")) << ", "
      << delay << ", "
      << (rand_min >= 0 ? std::to_string(rand_min) : "NULL") << ", "
      << (rand_max >= 0 ? std::to_string(rand_max) : "NULL") << ", "
      << (enabled ? "true" : "false") << ", "
      << (prefix_auto ? "true" : "false") << ", "
      << created_ts  << ", "
      << modified_ts << ", "
      << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << ") RETURNING id";

    if (!SQLDb->Exec(q, true)) {
        elog << "sqlSpamAction::insert> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    if (SQLDb->Tuples() > 0)
        id = atoi(SQLDb->GetValue(0, 0));
    return true;
}

bool sqlSpamAction::remove()
{
    stringstream q;
    q << "DELETE FROM spam_actions WHERE id = " << id;
    if (!SQLDb->Exec(q)) {
        elog << "sqlSpamAction::remove> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

} // namespace ds
} // namespace gnuworld
