/*
 * sqlUser.cc
 *
 * Stores a database user
 *
 * $Id: sqlUser.cc,v 1.7 2002/08/27 20:55:53 jeekay Exp $
 */
 
 #include <ctime>
 #include <sstream>
 
 #include "sqlUser.h"
 
 namespace gnuworld {
 
 namespace ns {
 
 const sqlUser::flagType sqlUser::F_SUSPEND  = 0x0001;
 const sqlUser::flagType sqlUser::F_AUTOKILL = 0x0002;
 const sqlUser::flagType sqlUser::F_RECOVER  = 0x0004;
 
 unsigned long int sqlUser::maxUserId = 0;
 
 /**
  * Default constructor.
  * This simply creates an empty, zeroed sqlUser
  */
sqlUser::sqlUser(sqlManager* _myManager) :
  id(0),
  name(""),
  flags(0),
  level(0),
  lastseen_ts(0),
  registered_ts(0),
  logmask(0)
{
  myManager = _myManager;
}

/**
 * Default destructor.
 * This does nothing as we have no heap space allocated
 */
sqlUser::~sqlUser()
{
}

/**
 * This function commits the current state of the sqlUser back to
 * the backend DB
 */
void sqlUser::commit()
{
  /* Use reference to sqlManager to queue a commit request */
  stringstream commitStatement;
  commitStatement << "UPDATE users SET"
    << " name = '" << name << "'"
    << ", flags = " << flags
    << ", level = " << level
    << ", lastseen_ts = " << lastseen_ts
    << ", registered_ts = " << registered_ts
    << " WHERE id = " << id
    << ends;
  myManager->queueCommit(commitStatement.str());
}

/**
 * This function updates this users lastseen back to the DB
 */
void sqlUser::commitLastSeen()
{
  lastseen_ts = time(NULL);
  commit();
}

/**
 * This function simply deletes this users entry from the DB.
 * It is possible to call insertUser() on a sqlUser that has
 * just been deleteUser()'d
 */
void sqlUser::deleteUser()
{
  /* Construct our delete statement */
  stringstream commitStatement;
  commitStatement << "DELETE FROM users WHERE"
    << " id = " << id
    << ends;
  myManager->queueCommit(commitStatement.str());
}

/**
 * This function inserts a brand new user into the DB.
 * It is a slight fudge, in that it first creates a blank record then
 * calls commit() to update the data fields for that record. This is done
 * so that any new fields added will automatically be dealt with in commit()
 * instead of in 50 different functions.
 */
void sqlUser::insertUser()
{
  /* Grab the next available user id */
  id = ++maxUserId;
  
  /* Construct our insert statement */
  stringstream commitStatement;
  commitStatement << "INSERT INTO users (id,name) VALUES ("
    << id
    << ", '" << name << "'"
    << ")"
    << ends;
  myManager->queueCommit(commitStatement.str());
  commit();
}

/**
 * This function loads in data from a DB handle,
 * to initialise the object properly
 */
void sqlUser::setAllMembers(PgDatabase* theDB, int row)
{
  /* Grab the data, set the members */
id = atoi(theDB->GetValue(row, 0));
name = theDB->GetValue(row, 1);
flags = atoi(theDB->GetValue(row, 2));
level = atoi(theDB->GetValue(row, 3));
lastseen_ts = atoi(theDB->GetValue(row, 4));
registered_ts = atoi(theDB->GetValue(row, 5));
logmask = atoi(theDB->GetValue(row, 6));

if(id > maxUserId) maxUserId = id;
}

} // namespace ns
 
} // namespace gnuworld
