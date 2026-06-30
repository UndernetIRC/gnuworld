/**
 * sqlSpamEvent.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <sstream>
#include "dbHandle.h"
#include "ELog.h"
#include "sqlSpamEvent.h"
#include "misc.h"

namespace gnuworld {
namespace ds {

using std::endl;
using std::stringstream;

sqlSpamEvent::sqlSpamEvent(dbHandle* _SQLDb)
    : id(0), name(), description(), event_type(), event_param(),
      target(31), case_sensitive(false), points(1), point_expiry(60),
      max_occurrence(-1), points_per("CLIENT"), requires_event_id(0), enabled(true),
      repeat_crossuser(false), repeat_min_count(2), repeat_exclusion_regex(),
      created_ts(0), modified_ts(0), modified_by(0),
      SQLDb(_SQLDb)
{}

sqlSpamEvent::~sqlSpamEvent() {}

// Column order matches the SELECT in preloadSpamEvents():
// id, name, description, event_type, event_param, target,
// case_sensitive, points, point_expiry, max_occurrence, requires_event_id, enabled,
// points_per, repeat_crossuser, repeat_min_count, repeat_exclusion_regex,
// created_ts, modified_ts, modified_by
void sqlSpamEvent::setAllMembers(int row)
{
    id                     = atoi(SQLDb->GetValue(row, 0).c_str());
    name                   = SQLDb->GetValue(row, 1);
    description            = SQLDb->GetValue(row, 2);
    event_type             = SQLDb->GetValue(row, 3);
    event_param            = SQLDb->GetValue(row, 4);
    target                 = atoi(SQLDb->GetValue(row, 5).c_str());
    case_sensitive         = (SQLDb->GetValue(row, 6) == "t");
    points                 = atoi(SQLDb->GetValue(row, 7).c_str());
    point_expiry           = atoi(SQLDb->GetValue(row, 8).c_str());
    const string maxOcc    = SQLDb->GetValue(row, 9);
    max_occurrence         = !maxOcc.empty() ? atoi(maxOcc.c_str()) : -1;
    const string reqEv     = SQLDb->GetValue(row, 10);
    requires_event_id      = !reqEv.empty() ? atoi(reqEv.c_str()) : 0;
    enabled                = (SQLDb->GetValue(row, 11) == "t");
    const string pp        = SQLDb->GetValue(row, 12);
    points_per             = !pp.empty() ? pp : "CLIENT";
    repeat_crossuser       = (SQLDb->GetValue(row, 13) == "t");
    const string rmc       = SQLDb->GetValue(row, 14);
    repeat_min_count       = !rmc.empty() ? atoi(rmc.c_str()) : 2;
    repeat_exclusion_regex = SQLDb->GetValue(row, 15);
    created_ts             = atoi(SQLDb->GetValue(row, 16).c_str());
    modified_ts            = atoi(SQLDb->GetValue(row, 17).c_str());
    const string modBy     = SQLDb->GetValue(row, 18);
    modified_by            = !modBy.empty() ? atoi(modBy.c_str()) : 0;
}

bool sqlSpamEvent::commit()
{
    stringstream q;
    q << "UPDATE spam_events SET "
      << "name = '"              << escapeSQLChars(name)               << "', "
      << "description = '"       << escapeSQLChars(description)        << "', "
      << "event_type = '"        << escapeSQLChars(event_type)         << "', "
      << "event_param = '"       << escapeSQLChars(event_param)        << "', "
      << "target = "             << target                             << ", "
      << "case_sensitive = "     << (case_sensitive ? "true" : "false") << ", "
      << "points = "             << points                             << ", "
      << "point_expiry = "       << point_expiry                       << ", "
      << "max_occurrence = "     << (max_occurrence >= 0 ? std::to_string(max_occurrence) : "NULL") << ", "
      << "points_per = '"        << escapeSQLChars(points_per)         << "', "
      << "requires_event_id = "  << (requires_event_id > 0 ? std::to_string(requires_event_id) : "NULL") << ", "
      << "enabled = "            << (enabled ? "true" : "false")       << ", "
      << "repeat_crossuser = "   << (repeat_crossuser ? "true" : "false") << ", "
      << "repeat_min_count = "   << repeat_min_count                   << ", "
      << "repeat_exclusion_regex = "
          << (repeat_exclusion_regex.empty() ? "NULL" : "'" + escapeSQLChars(repeat_exclusion_regex) + "'") << ", "
      << "modified_ts = "        << modified_ts                        << ", "
      << "modified_by = "        << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << " WHERE id = "          << id;

    if (!SQLDb->Exec(q)) {
        elog << "sqlSpamEvent::commit> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

bool sqlSpamEvent::insert()
{
    stringstream q;
    q << "INSERT INTO spam_events "
      << "(name, description, event_type, event_param, target, case_sensitive, "
      << "points, point_expiry, max_occurrence, points_per, requires_event_id, enabled, "
      << "repeat_crossuser, repeat_min_count, repeat_exclusion_regex, "
      << "created_ts, modified_ts, modified_by) VALUES ("
      << "'"  << escapeSQLChars(name)        << "', "
      << "'"  << escapeSQLChars(description) << "', "
      << "'"  << escapeSQLChars(event_type)  << "', "
      << "'"  << escapeSQLChars(event_param) << "', "
      << target                              << ", "
      << (case_sensitive ? "true" : "false") << ", "
      << points       << ", "
      << point_expiry << ", "
      << (max_occurrence >= 0 ? std::to_string(max_occurrence) : "NULL") << ", "
      << "'" << escapeSQLChars(points_per)   << "', "
      << (requires_event_id > 0 ? std::to_string(requires_event_id) : "NULL") << ", "
      << (enabled ? "true" : "false") << ", "
      << (repeat_crossuser ? "true" : "false") << ", "
      << repeat_min_count << ", "
      << (repeat_exclusion_regex.empty() ? "NULL" : "'" + escapeSQLChars(repeat_exclusion_regex) + "'") << ", "
      << created_ts   << ", "
      << modified_ts  << ", "
      << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << ") RETURNING id";

    if (!SQLDb->Exec(q, true)) {
        elog << "sqlSpamEvent::insert> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    if (SQLDb->Tuples() > 0)
        id = atoi(SQLDb->GetValue(0, 0));
    return true;
}

bool sqlSpamEvent::remove()
{
    stringstream q;
    q << "DELETE FROM spam_events WHERE id = " << id;
    if (!SQLDb->Exec(q)) {
        elog << "sqlSpamEvent::remove> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

} // namespace ds
} // namespace gnuworld
