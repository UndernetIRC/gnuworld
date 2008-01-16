/**
 * sqlcfUser.cc
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * $Id: sqlcfUser.cc,v 1.4 2008/01/16 02:03:40 buzlip01 Exp $
 */

#include	<sstream>
#include	<string>

#include	"dbHandle.h"

#include	"ELog.h"
#include	"misc.h"

#include	"chanfix.h"
#include	"sqlcfUser.h"

using namespace std;

namespace gnuworld
{

using std::stringstream;
using std::string;

namespace cf
{

const sqlcfUser::flagType sqlcfUser::F_SERVERADMIN =	0x01; /* +a */
const sqlcfUser::flagType sqlcfUser::F_BLOCK =		0x02; /* +b */
const sqlcfUser::flagType sqlcfUser::F_COMMENT =	0x04; /* +c */
const sqlcfUser::flagType sqlcfUser::F_CHANFIX =	0x08; /* +f */
const sqlcfUser::flagType sqlcfUser::F_OWNER =		0x10; /* +o */
const sqlcfUser::flagType sqlcfUser::F_USERMANAGER =	0x20; /* +u */
const sqlcfUser::flagType sqlcfUser::F_PERMBLOCKER =	0x80; /* +p */
const sqlcfUser::flagType sqlcfUser::F_LOGGEDIN =	0x40;

unsigned long int sqlcfUser::maxUserId = 0;

sqlcfUser::sqlcfUser(sqlManager* _myManager) :
  id(0),
  user_name(),
  created(0),
  last_seen(0),
  last_updated(0),
  last_updated_by(),
  language_id(0),
  group(),
  flags(0),
  isSuspended(false),
  useNotice(true),
  needOper(true)
{
  myManager = _myManager;
}

void sqlcfUser::setAllMembers(dbHandle* theDB, int row)
{
  id = atoi(theDB->GetValue(row, 0));
  user_name = theDB->GetValue(row, 1);
  created = atoi(theDB->GetValue(row, 2));
  last_seen = atoi(theDB->GetValue(row, 3));
  last_updated = atoi(theDB->GetValue(row, 4));
  last_updated_by = theDB->GetValue(row, 5);
  language_id = atoi(theDB->GetValue(row, 6));
  group = theDB->GetValue(row, 7);
  flags = atoi(theDB->GetValue(row, 8));
  isSuspended = (!strcasecmp(theDB->GetValue(row,9),"t") ? true : false);
  useNotice = (!strcasecmp(theDB->GetValue(row,10),"t") ? true : false);
  needOper = (!strcasecmp(theDB->GetValue(row,11),"t") ? true : false);

  if (id > maxUserId) maxUserId = id;
}

bool sqlcfUser::commit(dbHandle* cacheCon)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Create the UPDATE statement */
std::stringstream userCommit;
userCommit	<< "UPDATE users SET "
		<< "last_seen = " << last_seen << ", "
		<< "last_updated = " << last_updated << ", "
		<< "last_updated_by = '" << escapeSQLChars(last_updated_by) << "', "
		<< "language_id = " << language_id << ", "
		<< "faction = '" << escapeSQLChars(group) << "', "
		<< "flags = " << flags << ", "
		<< "issuspended = " << (isSuspended ? "'t'" : "'f'") << ", "
		<< "usenotice = " << (useNotice ? "'t'" : "'f'") << ", "
		<< "needoper = " << (needOper ? "'t'" : "'f'")
		<< " WHERE "
		<< "id = " << id
		;

if (!cacheCon->Exec(userCommit.str())) {
  elog	<< "sqlcfUser::commit> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
} else
  retval = true;

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return retval;
}

/**
 * This function inserts a brand new user into the DB.
 * It is a slight fudge, in that it first creates a blank record then
 * calls commit() to update the data fields for that record. This is done
 * so that any new fields added will automatically be dealt with in commit()
 * instead of in 50 different functions.
 */
bool sqlcfUser::Insert(dbHandle* cacheCon)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Grab the next available user id */
id = ++maxUserId;

/* Create the INSERT statement */
std::stringstream insertString;
insertString	<< "INSERT INTO users "
		<< "(id, user_name) "
		<< "VALUES "
		<< "("
		<< id << ", "
		<< "'" << escapeSQLChars(user_name) << "'"
		<< ")"
		;

if (!cacheCon->Exec(insertString.str())) {
  elog	<< "sqlcfUser::Insert> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
  maxUserId--;
} else
  retval = true;

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

if (retval)
  commit(cacheCon);

return retval;
} // sqlcfUser::Insert()

bool sqlcfUser::Delete(dbHandle* cacheCon)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Create the DELETE statement */
std::stringstream hostString;
hostString	<< "DELETE FROM hosts "
		<< "WHERE user_id = " << id
		;

if (!cacheCon->Exec(hostString.str())) {
  elog	<< "sqlcfUser::Delete> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
} else
  retval = true;
	
/* Create the DELETE statement */
std::stringstream deleteString;
deleteString	<< "DELETE FROM users "
		<< "WHERE id = '" << id << "'"
		;

if (!cacheCon->Exec(deleteString.str())) {
  elog	<< "sqlcfUser::Delete> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
} else
  retval = true;

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return retval;
}

void sqlcfUser::loadHostList(dbHandle* cacheCon)
{
/* Get a connection instance to our backend */

// dbHandle* cacheCon = myManager->getConnection();
// if (!cacheCon)
// 	elog << "[sqlcfUser::loadHostList() Could not get a connection to the database from the manager." << std::endl;


/* Retrieve the hosts */
std::stringstream theQuery;

theQuery	<< "SELECT host FROM hosts WHERE user_id = "
		<< id
		<< ends ;


if (cacheCon->Exec(theQuery.str(),true)) {
  // SQL Query succeeded
  for (unsigned int i = 0 ; i < cacheCon->Tuples(); i++) {
    hostList.push_back(cacheCon->GetValue(i, 0));
  }
}

/* Dispose of our connection instance */
// myManager->removeConnection(cacheCon);
return; 
}

bool sqlcfUser::addHost(dbHandle* cacheCon, const std::string& _theHost)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Create the INSERT statement */
std::stringstream insertString;
insertString    << "INSERT INTO hosts "
                << "(user_id, host) VALUES "
                << "("
                << id
                << ", '"
                << escapeSQLChars(_theHost).c_str()
                << "')"
                ;

if (!cacheCon->Exec(insertString.str())) {
  elog	<< "sqlcfUser::addHost> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
} else
  retval = true;

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

hostList.push_back(_theHost);

return retval;
}

bool sqlcfUser::delHost(dbHandle* cacheCon, const std::string& _theHost)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Create the DELETE statement */
std::stringstream deleteString;
deleteString	<< "DELETE FROM hosts "
		<< "WHERE user_id = "
		<< id
		<< " AND lower(host) = '"
		<< escapeSQLChars(string_lower(_theHost)).c_str()
		<< "'"
		;

if (!cacheCon->Exec(deleteString.str())) {
  elog	<< "sqlcfUser::delHost> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
} else
  retval = true;

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

if (hostList.size() < 1) return false;
hostListType::iterator ptr = find( hostList.begin(), hostList.end(), string_lower(_theHost) );
if (ptr == hostList.end()) return false;
hostList.erase(ptr);

return retval;
}

bool sqlcfUser::matchHost(const std::string& _host)
{
  if (hostList.size() < 1) return false;
  for(hostListType::iterator itr = hostList.begin() ;
    itr != hostList.end() ; ++itr) {
      if (match(*itr,_host) == 0) return true;
  }
  return false;
}

bool sqlcfUser::hasHost(const std::string& _host)
{
  if (hostList.size() < 1) return false;
  hostListType::iterator ptr = find( hostList.begin(), hostList.end(), string_lower(_host) );
  if ( ptr == hostList.end() ) return false;
  return true;
}

sqlcfUser::~sqlcfUser()
{
// No heap space allocated
}

} // namespace cf

} // namespace gnuworld
