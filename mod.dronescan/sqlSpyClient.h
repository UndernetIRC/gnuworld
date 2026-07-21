/**
 * sqlSpyClient.h
 *
 * Represents a row in the spyclients table.
 * Spy clients are fake IRC clients introduced to the network by dronescan
 * to monitor channels for spam. Unlike the legacy fakeclients table, these
 * use a standard IP string and carry all fields needed to introduce a P10
 * client (modes, account, etc.).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef SQLSPYCLIENT_H
#define SQLSPYCLIENT_H

#include <string>
#include "dbHandle.h"

namespace gnuworld {
namespace ds {

using std::string;

class sqlSpyClient {
  public:
    sqlSpyClient(dbHandle*);
    virtual ~sqlSpyClient();

    /* Accessors */
    inline int           getId()          const { return id; }
    inline const string& getNickname()    const { return nickname; }
    inline const string& getUsername()    const { return username; }
    inline const string& getHostname()    const { return hostname; }
    inline const string& getIp()          const { return ip; }
    inline const string& getRealname()    const { return realname; }
    inline const string& getAccount()     const { return account; }
    inline int           getAccountId()   const { return account_id; }
    inline const string& getModes()       const { return modes; }
    inline bool          isEnabled()      const { return enabled; }
    inline int           getCreatedBy()   const { return created_by; }
    inline int           getCreatedTs()   const { return created_ts; }
    inline int           getModifiedTs()  const { return modified_ts; }
    inline int           getModifiedBy()  const { return modified_by; }

    /* Mutators */
    inline void setId(int v)                   { id = v; }
    inline void setNickname(const string& v)   { nickname = v; }
    inline void setUsername(const string& v)   { username = v; }
    inline void setHostname(const string& v)   { hostname = v; }
    inline void setIp(const string& v)         { ip = v; }
    inline void setRealname(const string& v)   { realname = v; }
    inline void setAccount(const string& v)    { account = v; }
    inline void setAccountId(int v)            { account_id = v; }
    inline void setModes(const string& v)      { modes = v; }
    inline void setEnabled(bool v)             { enabled = v; }
    inline void setCreatedBy(int v)            { created_by = v; }
    inline void setCreatedTs(int v)            { created_ts = v; }
    inline void setModifiedTs(int v)           { modified_ts = v; }
    inline void setModifiedBy(int v)           { modified_by = v; }

    void setAllMembers(int row);
    bool commit();
    bool insert();
    bool remove();

  protected:
    int    id;
    string nickname;
    string username;
    string hostname;
    string ip;           // standard IPv4/IPv6 string (e.g. "1.2.3.4")
    string realname;
    string account;      // empty string means NULL / not authenticated
    int    account_id;   // 0 means NULL / not authenticated
    string modes;        // IRC user modes at introduce time (e.g. "+i")
    bool   enabled;
    int    created_by;   // 0 means NULL
    int    created_ts;
    int    modified_ts;
    int    modified_by;  // 0 means NULL

    dbHandle* SQLDb;
};

} // namespace ds
} // namespace gnuworld

#endif // SQLSPYCLIENT_H
