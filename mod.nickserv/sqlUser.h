#ifndef _SQLUSER_H
#define _SQLUSER_H "$Id: sqlUser.h,v 1.7 2002/08/25 16:21:44 jeekay Exp $"

#include <string>

#include "libpq++.h"

#include "sqlManager.h"

namespace gnuworld
{

namespace ns
{

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
    inline bool getFlag(const flagType& whichFlag) const
      { return (whichFlag == (flags & whichFlag)); }
    
    /** Get the user level */
    inline unsigned int getLevel() const
       { return level; }
    
    /** Get the user lastseen */
    inline unsigned int getLastSeenTS() const
      { return lastseen_ts; }
    
    /** Get the registered ts */
    inline unsigned int getRegisteredTS() const
      { return registered_ts; }
    
    
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
    
    /** Set the user level */
    inline void setLevel(const unsigned int _level)
      { level = _level; }
    
    /** Set the lastseen */
    inline void setLastSeenTS(const unsigned int _lastseen)
      { lastseen_ts = _lastseen; }
    
    /** Set the registered ts */
    inline void setRegisteredTS(const unsigned int _registered)
      { registered_ts = _registered; }
    
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
    void setAllMembers(PgDatabase*, int);

    /** Static member for keeping track of max user id */
    static unsigned long int maxUserId;
    
  protected:
    unsigned int id;
    string name;
    flagType flags;
    unsigned int level;
    unsigned int lastseen_ts;
    unsigned int registered_ts;
    
    sqlManager* myManager;
}; // class sqlUser

} // namespace ns

} // namespace gnuworld

#endif
