#ifndef _SQLUSER_H
#define _SQLUSER_H "$Id: sqlUser.h,v 1.2 2002/08/10 15:44:24 jeekay Exp $"

#include <string>

#include "libpq++.h"

namespace gnuworld
{

namespace ns
{

class sqlManager;

class sqlUser
{

typedef unsigned short int flagType;
static const flagType F_SUSPEND;
static const flagType F_AUTOKILL;

  public:
    /** Default constructor makes an empty user */
    sqlUser();
    
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
    
    
    /* MISCELLANEOUS METHODS */
    
    /** Commit this user back to the database */
    void commit();
    
    /** Load data for this user from a given DB handle */
    void setAllMembers(PgDatabase*, int);

  protected:
    unsigned int id;
    string name;
    flagType flags;
    unsigned int level;
    
    sqlManager* myManager;
}; // class sqlUser

} // namespace ns

} // namespace gnuworld

#endif
