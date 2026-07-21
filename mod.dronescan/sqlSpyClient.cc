/**
 * sqlSpyClient.cc
 *
 * Implementation of the sqlSpyClient class.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <sstream>
#include "dbHandle.h"
#include "ELog.h"
#include "sqlSpyClient.h"
#include "misc.h"

namespace gnuworld {
namespace ds {

using std::endl;
using std::stringstream;

sqlSpyClient::sqlSpyClient(dbHandle* _SQLDb)
    : id(0), nickname(), username(), hostname(), ip(), realname(),
      account(), account_id(0), modes("+i"), enabled(true),
      created_by(0), created_ts(0), modified_ts(0), modified_by(0),
      SQLDb(_SQLDb)
{}

sqlSpyClient::~sqlSpyClient() {}

// Column order matches the SELECT in preloadSpyClients():
// id, nickname, username, hostname, ip, realname,
// account, account_id, modes, enabled,
// created_by, created_ts, modified_ts, modified_by
void sqlSpyClient::setAllMembers(int row)
{
    id                = atoi(SQLDb->GetValue(row, 0).c_str());
    nickname          = SQLDb->GetValue(row, 1);
    username          = SQLDb->GetValue(row, 2);
    hostname          = SQLDb->GetValue(row, 3);
    ip                = SQLDb->GetValue(row, 4);
    realname          = SQLDb->GetValue(row, 5);
    account           = SQLDb->GetValue(row, 6);  // empty string if NULL
    const string aid  = SQLDb->GetValue(row, 7);
    account_id        = !aid.empty() ? atoi(aid.c_str()) : 0;
    modes             = SQLDb->GetValue(row, 8);
    enabled           = (SQLDb->GetValue(row, 9) == "t");
    const string cb   = SQLDb->GetValue(row, 10);
    created_by        = !cb.empty() ? atoi(cb.c_str()) : 0;
    created_ts        = atoi(SQLDb->GetValue(row, 11).c_str());
    modified_ts       = atoi(SQLDb->GetValue(row, 12).c_str());
    const string modBy = SQLDb->GetValue(row, 13);
    modified_by       = !modBy.empty() ? atoi(modBy.c_str()) : 0;
}

bool sqlSpyClient::commit()
{
    stringstream q;
    q << "UPDATE spyclients SET "
      << "nickname = '"     << escapeSQLChars(nickname)  << "', "
      << "username = '"     << escapeSQLChars(username)  << "', "
      << "hostname = '"     << escapeSQLChars(hostname)  << "', "
      << "ip = '"           << escapeSQLChars(ip)        << "', "
      << "realname = '"     << escapeSQLChars(realname)  << "', "
      << "account = "       << (account.empty() ? "NULL" : "'" + escapeSQLChars(account) + "'") << ", "
      << "account_id = "    << account_id                << ", "
      << "modes = '"        << escapeSQLChars(modes)     << "', "
      << "enabled = "       << (enabled ? "true" : "false") << ", "
      << "modified_ts = "   << modified_ts               << ", "
      << "modified_by = "   << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << " WHERE id = "     << id;

    if (!SQLDb->Exec(q)) {
        elog << "sqlSpyClient::commit> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

bool sqlSpyClient::insert()
{
    stringstream q;
    q << "INSERT INTO spyclients "
      << "(nickname, username, hostname, ip, realname, account, account_id, "
      << "modes, enabled, created_by, created_ts, modified_ts, modified_by) VALUES ("
      << "'"  << escapeSQLChars(nickname) << "', "
      << "'"  << escapeSQLChars(username) << "', "
      << "'"  << escapeSQLChars(hostname) << "', "
      << "'"  << escapeSQLChars(ip)       << "', "
      << "'"  << escapeSQLChars(realname) << "', "
      << (account.empty() ? "NULL" : "'" + escapeSQLChars(account) + "'") << ", "
      << account_id << ", "
      << "'"  << escapeSQLChars(modes)    << "', "
      << (enabled ? "true" : "false") << ", "
      << (created_by > 0 ? std::to_string(created_by) : "NULL") << ", "
      << created_ts  << ", "
      << modified_ts << ", "
      << (modified_by > 0 ? std::to_string(modified_by) : "NULL")
      << ") RETURNING id";

    if (!SQLDb->Exec(q, true)) {
        elog << "sqlSpyClient::insert> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    if (SQLDb->Tuples() > 0)
        id = atoi(SQLDb->GetValue(0, 0));
    return true;
}

bool sqlSpyClient::remove()
{
    stringstream q;
    q << "DELETE FROM spyclients WHERE id = " << id;
    if (!SQLDb->Exec(q)) {
        elog << "sqlSpyClient::remove> " << SQLDb->ErrorMessage() << endl;
        return false;
    }
    return true;
}

} // namespace ds
} // namespace gnuworld
