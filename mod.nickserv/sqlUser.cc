/*
 * sqlUser.cc
 *
 * Stores a database user
 *
 * $Id: sqlUser.cc,v 1.1 2002/08/10 13:55:19 jeekay Exp $
 */
 
 #include "sqlUser.h"
 
 namespace gnuworld {
 
 namespace ns {
 
 const sqlUser::flagType sqlUser::F_SUSPEND = 0x01;
 
 /**
  * Default constructor.
  * This simply creates an empty, zeroed sqlUser
  */
sqlUser::sqlUser() :
  id(0),
  name(""),
  flags(0)
{
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
}

} // namespace ns
 
} // namespace gnuworld
