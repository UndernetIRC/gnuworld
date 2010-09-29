/**
 * sqlUser.h
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */

#ifndef SQLUSER_H
#define SQLUSER_H

#include <string>

#include "dbHandle.h"

#include "logTarget.h"

namespace gnuworld
{

namespace ns
{

using std::string ;

class sqlManager;

class sqlUser
{
  public:

    typedef unsigned short int flagType;
    static const flagType F_SUSPEND;
    static const flagType F_AUTOKILL;
    static const flagType F_RECOVER;

    /** Default constructor makes an empty user */
    sqlUser(sqlManager*);

    /** Default destructor deletes user */
    ~sqlUser();


    /* ACCESSOR METHODS */

    /** Accessor for id */
    inline const unsigned int& getID() const
      { return id; }

    /** Accessor for name */
    inline const string& getName() const
      { return name; }

    /** Check whether a user has a given flag set */
    inline bool hasFlag(const flagType& whichFlag) const
      { return (flags & whichFlag); }

    /** Get all the flags */
    inline unsigned short int getFlags() const
      { return flags; }

    /** Get the user level */
    inline unsigned int getLevel() const
       { return level; }

    /** Get the user lastseen */
    inline unsigned int getLastSeenTS() const
      { return lastseen_ts; }

    /** Get the registered ts */
    inline unsigned int getRegisteredTS() const
      { return registered_ts; }

    /** Get the log mask */
    inline logging::events::eventType getLogMask() const
      { return logmask; }


    /* MUTATOR METHODS */

    /** Mutator for name */
    inline void setName(const string& _name)
      { name = _name; }

    /** Remove a flag from the user record */
    inline void removeFlag(const flagType& whichFlag)
      { flags &= ~whichFlag; }

    /** Set a flag on the user record */
    inline void setFlag(const flagType& whichFlag)
      { flags |= whichFlag; }

    /** Set all flags */
    inline void setFlags(const flagType& whichFlags)
      { flags = whichFlags; }

    /** Set the user level */
    inline void setLevel(const unsigned int _level)
      { level = _level; }

    /** Set the lastseen */
    inline void setLastSeenTS(const unsigned int _lastseen)
      { lastseen_ts = _lastseen; }

    /** Set the registered ts */
    inline void setRegisteredTS(const unsigned int _registered)
      { registered_ts = _registered; }

    /** Set the log mask */
    inline void setLogMask(const logging::events::eventType _logmask)
      { logmask = _logmask; }

    /* MISCELLANEOUS METHODS */

    /** Commit this user back to the database */
    void commit();

    /** Update this users lastseen */
    void commitLastSeen();

    /** Delete this user from the database */
    void deleteUser();

    /** Insert a brand new sqlUser into the database */
    void insertUser();

    /** Load data for this user from a given DB handle */
    void setAllMembers(dbHandle*, int);

    /** Static member for keeping track of max user id */
    static unsigned long int maxUserId;

  protected:
    unsigned int id;
    string name;
    flagType flags;
    unsigned int level;
    unsigned int lastseen_ts;
    unsigned int registered_ts;
    logging::events::eventType logmask;

    sqlManager* myManager;
}; // class sqlUser

} // namespace ns

} // namespace gnuworld

#endif
