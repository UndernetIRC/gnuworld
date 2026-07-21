/**
 * sqlSpamExclusion.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <sstream>
#include "dbHandle.h"
#include "ELog.h"
#include "sqlSpamExclusion.h"
#include "misc.h"

namespace gnuworld {
namespace ds {

using std::endl;
using std::stringstream;

sqlSpamExclusion::sqlSpamExclusion(dbHandle* _SQLDb)
    : id(0), exclusion_type(), value(),
      created_ts(0), modified_ts(0), modified_by(0),
      SQLDb(_SQLDb)
{}

sqlSpamExclusion::~sqlSpamExclusion() {}

// Column order matches the SELECT in preloadSpamExclusions():
// id, exclusion_type, value, created_ts, modified_ts, modified_by
void sqlSpamExclusion::setAllMembers(int row)
{
    id             = atoi(SQLDb->GetValue(row, 0).c_str());
    exclusion_type = SQLDb->GetValue(row, 1);
    value          = SQLDb->GetValue(row, 2);
    created_ts     = atoi(SQLDb->GetValue(row, 3).c_str());
    modified_ts    = atoi(SQLDb->GetValue(row, 4).c_str());
    const string modBy = SQLDb->GetValue(row, 5);
    modified_by    = !modBy.empty() ? atoi(modBy.c_str()) : 0;
}

bool sqlSpamExclusion::commit()
{
    stringstream q;
    q << "UPDATE spam_exclusions SET "
      << "exclusion_type = '" << escapeSQLChars(exclusion_type) << "', "
      << "value = '"          << escapeSQLChars(value)          << "', "
      << "modified_ts = "     << modified_ts                    << ", "
      << "modified_by = "     << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << " WHERE id = "       << id;

    if (!SQLDb->Exec(q)) {
        elog << "sqlSpamExclusion::commit> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

bool sqlSpamExclusion::insert()
{
    stringstream q;
    q << "INSERT INTO spam_exclusions "
      << "(exclusion_type, value, created_ts, modified_ts, modified_by) VALUES ("
      << "'" << escapeSQLChars(exclusion_type) << "', "
      << "'" << escapeSQLChars(value)          << "', "
      << created_ts  << ", "
      << modified_ts << ", "
      << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << ") RETURNING id";

    if (!SQLDb->Exec(q, true)) {
        elog << "sqlSpamExclusion::insert> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    if (SQLDb->Tuples() > 0)
        id = atoi(SQLDb->GetValue(0, 0));
    return true;
}

bool sqlSpamExclusion::remove()
{
    stringstream q;
    q << "DELETE FROM spam_exclusions WHERE id = " << id;
    if (!SQLDb->Exec(q)) {
        elog << "sqlSpamExclusion::remove> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

} // namespace ds
} // namespace gnuworld
