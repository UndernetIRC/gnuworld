/*
 * sqlUser.cc
 *
 * Stores a database user
 *
 * $Id: sqlUser.cc,v 1.4 2002/08/23 21:25:25 jeekay Exp $
 */
 
 #include <sstream>
 
 #include "sqlUser.h"
 
 namespace gnuworld {
 
 namespace ns {
 
 const sqlUser::flagType sqlUser::F_SUSPEND  = 0x0001;
 const sqlUser::flagType sqlUser::F_AUTOKILL = 0x0002;
 
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
  registered_ts(0)
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
    << " flags = " << flags
    << " level = " << level
    << " lastseen_ts = " << lastseen_ts
    << " registered_ts = " << registered_ts
    << " WHERE id = " << id
    << ends;
  myManager->queueCommit(commitStatement.str());
}

/**
 * This function updates this users lastseen back to the DB
 */
void sqlUser::commitLastSeen()
{
  stringstream commitStatement;
  commitStatement << "UPDATE users SET"
    << " lastseen_ts = now()::abstime::int4"
    << " WHERE id = " << id
    << ends;
  myManager->queueCommit(commitStatement.str());
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
}

} // namespace ns
 
} // namespace gnuworld
