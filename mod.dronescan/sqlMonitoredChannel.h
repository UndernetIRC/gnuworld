/**
 * sqlMonitoredChannel.h
 *
 * Represents a row in the monitored_channels table.
 * Monitored channels are IRC channels that dronescan actively watches for
 * spam by joining them with spy clients or with the main xClient bot.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef SQLMONITOREDCHANNEL_H
#define SQLMONITOREDCHANNEL_H

#include <string>
#include "dbHandle.h"

namespace gnuworld {
namespace ds {

using std::string;

class sqlMonitoredChannel {
  public:
    sqlMonitoredChannel(dbHandle*);
    virtual ~sqlMonitoredChannel();

    /* Accessors */
    inline int           getId()           const { return id; }
    inline const string& getName()         const { return name; }
    // forcejoin: join even if +i/+k/+l(full) or if spy client is banned
    inline bool          isForceJoin()     const { return forcejoin; }
    // joinasservice: monitor via the main xClient bot; no spy client spawned
    inline bool          isJoinAsService() const { return joinasservice; }
    inline bool          isEnabled()       const { return enabled; }
    inline int           getCreatedTs()    const { return created_ts; }
    inline int           getModifiedTs()   const { return modified_ts; }
    inline int           getModifiedBy()   const { return modified_by; }
    // 0 means never triggered
    inline int           getLastTriggeredTs()   const { return last_triggered_ts; }
    inline const string& getLastTriggeredRule() const { return last_triggered_rule; }

    /* Mutators */
    inline void setId(int v)                   { id = v; }
    inline void setName(const string& v)       { name = v; }
    inline void setForceJoin(bool v)           { forcejoin = v; }
    inline void setJoinAsService(bool v)       { joinasservice = v; }
    inline void setEnabled(bool v)             { enabled = v; }
    inline void setCreatedTs(int v)            { created_ts = v; }
    inline void setModifiedTs(int v)           { modified_ts = v; }
    inline void setModifiedBy(int v)           { modified_by = v; }
    inline void setLastTriggeredTs(int v)          { last_triggered_ts = v; }
    inline void setLastTriggeredRule(const string& v) { last_triggered_rule = v; }

    void setAllMembers(int row);
    bool commit();
    bool insert();
    bool remove();

  protected:
    int    id;
    string name;
    bool   forcejoin;
    bool   joinasservice;
    bool   enabled;
    int    created_ts;
    int    modified_ts;
    int    modified_by;   // 0 means NULL
    int    last_triggered_ts;   // 0 means NULL (never triggered)
    string last_triggered_rule;

    dbHandle* SQLDb;
};

} // namespace ds
} // namespace gnuworld

#endif // SQLMONITOREDCHANNEL_H
