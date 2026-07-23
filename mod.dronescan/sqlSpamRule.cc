/**
 * sqlSpamRule.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <sstream>
#include "dbHandle.h"
#include "ELog.h"
#include "sqlSpamRule.h"
#include "misc.h"

namespace gnuworld {
namespace ds {

using std::endl;
using std::stringstream;

sqlSpamRule::sqlSpamRule(dbHandle* _SQLDb)
    : id(0), name(), description(), threshold(10),
      wait_on_rule_id(0), allchans(true), points_per("CLIENT"),
      score_globally(false),
      enabled(true), silent(false), report_source("BOT"),
      created_ts(0), modified_ts(0), modified_by(0),
      SQLDb(_SQLDb)
{}

sqlSpamRule::~sqlSpamRule() {}

// Column order matches the SELECT in preloadSpamRules():
// id, name, description, threshold, wait_on_rule_id,
// allchans, points_per, score_globally, enabled, silent, report_source,
// created_ts, modified_ts, modified_by
void sqlSpamRule::setAllMembers(int row)
{
    id                   = atoi(SQLDb->GetValue(row, 0).c_str());
    name                 = SQLDb->GetValue(row, 1);
    description          = SQLDb->GetValue(row, 2);
    threshold            = atoi(SQLDb->GetValue(row, 3).c_str());
    const string wor     = SQLDb->GetValue(row, 4);
    wait_on_rule_id      = !wor.empty() ? atoi(wor.c_str()) : 0;
    allchans             = (SQLDb->GetValue(row, 5) == "t");
    const string pp      = SQLDb->GetValue(row, 6);
    points_per           = !pp.empty() ? pp : "CLIENT";
    score_globally       = (SQLDb->GetValue(row, 7) == "t");
    enabled              = (SQLDb->GetValue(row, 8) == "t");
    silent               = (SQLDb->GetValue(row, 9) == "t");
    const string rs      = SQLDb->GetValue(row, 10);
    report_source        = !rs.empty() ? rs : "BOT";
    created_ts           = atoi(SQLDb->GetValue(row, 11).c_str());
    modified_ts          = atoi(SQLDb->GetValue(row, 12).c_str());
    const string modBy   = SQLDb->GetValue(row, 13);
    modified_by          = !modBy.empty() ? atoi(modBy.c_str()) : 0;
}

bool sqlSpamRule::commit()
{
    stringstream q;
    q << "UPDATE spam_rules SET "
      << "name = '"              << escapeSQLChars(name)        << "', "
      << "description = '"       << escapeSQLChars(description) << "', "
      << "threshold = "          << threshold << ", "
      << "wait_on_rule_id = "    << (wait_on_rule_id > 0 ? std::to_string(wait_on_rule_id) : "NULL") << ", "
      << "allchans = "           << (allchans ? "true" : "false") << ", "
      << "points_per = "         << "'" << escapeSQLChars(points_per) << "', "
      << "score_globally = "     << (score_globally ? "true" : "false") << ", "
      << "enabled = "            << (enabled ? "true" : "false") << ", "
      << "silent = "             << (silent ? "true" : "false") << ", "
      << "report_source = "      << "'" << escapeSQLChars(report_source) << "', "
      << "modified_ts = "        << modified_ts << ", "
      << "modified_by = "        << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << " WHERE id = "          << id;

    if (!SQLDb->Exec(q)) {
        elog << "sqlSpamRule::commit> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

bool sqlSpamRule::insert()
{
    stringstream q;
    q << "INSERT INTO spam_rules "
      << "(name, description, threshold, wait_on_rule_id, allchans, "
      << "points_per, score_globally, enabled, silent, report_source, "
      << "created_ts, modified_ts, modified_by) VALUES ("
      << "'"  << escapeSQLChars(name)        << "', "
      << "'"  << escapeSQLChars(description) << "', "
      << threshold << ", "
      << (wait_on_rule_id > 0 ? std::to_string(wait_on_rule_id) : "NULL") << ", "
      << (allchans ? "true" : "false") << ", "
      << "'" << escapeSQLChars(points_per) << "', "
      << (score_globally ? "true" : "false") << ", "
      << (enabled ? "true" : "false") << ", "
      << (silent  ? "true" : "false") << ", "
      << "'" << escapeSQLChars(report_source) << "', "
      << created_ts  << ", "
      << modified_ts << ", "
      << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << ") RETURNING id";

    if (!SQLDb->Exec(q, true)) {
        elog << "sqlSpamRule::insert> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    if (SQLDb->Tuples() > 0)
        id = atoi(SQLDb->GetValue(0, 0));
    return true;
}

bool sqlSpamRule::remove()
{
    stringstream q;
    q << "DELETE FROM spam_rules WHERE id = " << id;
    if (!SQLDb->Exec(q)) {
        elog << "sqlSpamRule::remove> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

} // namespace ds
} // namespace gnuworld
