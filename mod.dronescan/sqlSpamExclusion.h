/**
 * sqlSpamExclusion.h
 *
 * Represents a row in the spam_exclusions table.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef SQLSPAMEXCLUSION_H
#define SQLSPAMEXCLUSION_H

#include <string>
#include "dbHandle.h"

namespace gnuworld {
namespace ds {

using std::string;

class sqlSpamExclusion {
  public:
    sqlSpamExclusion(dbHandle*);
    virtual ~sqlSpamExclusion();

    /* Accessors */
    inline int           getId()             const { return id; }
    inline const string& getExclusionType() const { return exclusion_type; }
    inline const string& getValue()         const { return value; }
    inline int           getCreatedTs()     const { return created_ts; }
    inline int           getModifiedTs()    const { return modified_ts; }
    inline int           getModifiedBy()    const { return modified_by; }

    /* Mutators */
    inline void setId(int v)                    { id = v; }
    inline void setExclusionType(const string& v){ exclusion_type = v; }
    inline void setValue(const string& v)       { value = v; }
    inline void setCreatedTs(int v)             { created_ts = v; }
    inline void setModifiedTs(int v)            { modified_ts = v; }
    inline void setModifiedBy(int v)            { modified_by = v; }

    void setAllMembers(int row);
    bool insert();
    bool remove();

  protected:
    int    id;
    string exclusion_type;
    string value;
    int    created_ts;
    int    modified_ts;
    int    modified_by; // 0 means NULL

    dbHandle* SQLDb;
};

} // namespace ds
} // namespace gnuworld

#endif
