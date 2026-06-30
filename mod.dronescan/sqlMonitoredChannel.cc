/**
 * sqlMonitoredChannel.cc
 *
 * Implementation of the sqlMonitoredChannel class.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <sstream>
#include "dbHandle.h"
#include "ELog.h"
#include "sqlMonitoredChannel.h"
#include "misc.h"

namespace gnuworld {
namespace ds {

using std::endl;
using std::stringstream;

sqlMonitoredChannel::sqlMonitoredChannel(dbHandle* _SQLDb)
    : id(0), name(), forcejoin(false), joinasservice(false),
      enabled(true), created_ts(0), modified_ts(0), modified_by(0),
      SQLDb(_SQLDb)
{}

sqlMonitoredChannel::~sqlMonitoredChannel() {}

// Column order matches the SELECT in preloadMonitoredChannels():
// id, name, forcejoin, joinasservice, enabled,
// created_ts, modified_ts, modified_by
void sqlMonitoredChannel::setAllMembers(int row)
{
    id           = atoi(SQLDb->GetValue(row, 0).c_str());
    name         = SQLDb->GetValue(row, 1);
    forcejoin    = (SQLDb->GetValue(row, 2) == "t");
    joinasservice = (SQLDb->GetValue(row, 3) == "t");
    enabled      = (SQLDb->GetValue(row, 4) == "t");
    created_ts   = atoi(SQLDb->GetValue(row, 5).c_str());
    modified_ts  = atoi(SQLDb->GetValue(row, 6).c_str());
    const string modBy = SQLDb->GetValue(row, 7);
    modified_by  = !modBy.empty() ? atoi(modBy.c_str()) : 0;
}

bool sqlMonitoredChannel::commit()
{
    stringstream q;
    q << "UPDATE monitored_channels SET "
      << "name = '"          << escapeSQLChars(name)                   << "', "
      << "forcejoin = "      << (forcejoin ? "true" : "false")         << ", "
      << "joinasservice = "  << (joinasservice ? "true" : "false")     << ", "
      << "enabled = "        << (enabled ? "true" : "false")           << ", "
      << "modified_ts = "    << modified_ts                            << ", "
      << "modified_by = "    << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << " WHERE id = "      << id;

    if (!SQLDb->Exec(q)) {
        elog << "sqlMonitoredChannel::commit> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

bool sqlMonitoredChannel::insert()
{
    stringstream q;
    q << "INSERT INTO monitored_channels "
      << "(name, forcejoin, joinasservice, enabled, created_ts, modified_ts, modified_by) VALUES ("
      << "'"  << escapeSQLChars(name)                       << "', "
      << (forcejoin ? "true" : "false")                     << ", "
      << (joinasservice ? "true" : "false")                 << ", "
      << (enabled ? "true" : "false")                       << ", "
      << created_ts  << ", "
      << modified_ts << ", "
      << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << ") RETURNING id";

    if (!SQLDb->Exec(q, true)) {
        elog << "sqlMonitoredChannel::insert> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    if (SQLDb->Tuples() > 0)
        id = atoi(SQLDb->GetValue(0, 0));
    return true;
}

bool sqlMonitoredChannel::remove()
{
    stringstream q;
    q << "DELETE FROM monitored_channels WHERE id = " << id;
    if (!SQLDb->Exec(q)) {
        elog << "sqlMonitoredChannel::remove> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

} // namespace ds
} // namespace gnuworld
